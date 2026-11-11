/*
 * 9번: POSIX 메시지 큐 채팅
 *
 * 사용법: ./bin/09_mq_chat alice|bob
 *        ./bin/09_mq_chat --clean
 */
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MQ_A2B "/lab2_chat_a2b"
#define MQ_B2A "/lab2_chat_b2a"
#define MAX_MESSAGE 256

static void clean_queues(void) {
    if (mq_unlink(MQ_A2B) == -1 && errno != ENOENT) {
        perror("mq_unlink(" MQ_A2B ")");
    }
    if (mq_unlink(MQ_B2A) == -1 && errno != ENOENT) {
        perror("mq_unlink(" MQ_B2A ")");
    }
}

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "--clean") == 0) {
        clean_queues();
        printf("메시지 큐를 정리했습니다.\n");
        return EXIT_SUCCESS;
    }

    if (argc != 2 || (strcmp(argv[1], "alice") != 0 && strcmp(argv[1], "bob") != 0)) {
        fprintf(stderr, "사용법: %s alice|bob\n", argv[0]);
        fprintf(stderr, "       %s --clean\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *role = argv[1];
    const char *in_name;
    const char *out_name;

    if (strcmp(role, "alice") == 0) {
        in_name = MQ_B2A;
        out_name = MQ_A2B;
    } else {
        in_name = MQ_A2B;
        out_name = MQ_B2A;
    }

    struct mq_attr attr;
    memset(&attr, 0, sizeof(attr));
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MESSAGE;

    mqd_t in_queue = mq_open(in_name, O_CREAT | O_RDWR, 0644, &attr);
    if (in_queue == (mqd_t)-1) {
        perror("mq_open(in)");
        return EXIT_FAILURE;
    }

    mqd_t out_queue = mq_open(out_name, O_CREAT | O_RDWR, 0644, &attr);
    if (out_queue == (mqd_t)-1) {
        perror("mq_open(out)");
        mq_close(in_queue);
        return EXIT_FAILURE;
    }

    printf("[%s] 메시지 큐 채팅을 시작합니다. 종료하려면 quit을 입력하세요.\n", role);
    fflush(stdout);

    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[1].fd = (int)in_queue;
    fds[1].events = POLLIN;

    int running = 1;
    while (running) {
        int ready = poll(fds, 2, -1);
        if (ready == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("poll");
            break;
        }

        /* 키보드 입력 → 상대방 큐로 전송 */
        if (fds[0].revents & POLLIN) {
            char line[MAX_MESSAGE];
            if (fgets(line, sizeof(line), stdin) == NULL) {
                break;
            }

            if (strcmp(line, "quit\n") == 0 || strcmp(line, "quit") == 0) {
                break;
            }

            if (mq_send(out_queue, line, strlen(line) + 1, 1) == -1) {
                perror("mq_send");
                break;
            }
        }

        /* 들어오는 큐의 메시지 → 화면에 출력 */
        if (fds[1].revents & POLLIN) {
            char buffer[MAX_MESSAGE + 1];
            ssize_t count = mq_receive(in_queue, buffer, MAX_MESSAGE, NULL);
            if (count >= 0) {
                buffer[count] = '\0';
                printf("상대: %s", buffer);
                fflush(stdout);
            } else {
                perror("mq_receive");
                running = 0;
            }
        }
    }

    mq_close(in_queue);
    mq_close(out_queue);

    /* 양쪽 모두 정리하므로 이미 삭제된 경우(ENOENT)는 무시한다. */
    clean_queues();

    printf("[%s] 메시지 큐 채팅을 종료합니다.\n", role);
    return EXIT_SUCCESS;
}
