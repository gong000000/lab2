/*
 * 8번: 파이프 채팅
 *
 * 사용법: ./bin/08_pipe_chat alice|bob
 */
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_A2B "/tmp/lab2_chat_a2b"
#define FIFO_B2A "/tmp/lab2_chat_b2a"

static void make_fifo(const char *path) {
    if (mkfifo(path, 0600) == -1 && errno != EEXIST) {
        perror(path);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2 || (strcmp(argv[1], "alice") != 0 && strcmp(argv[1], "bob") != 0)) {
        fprintf(stderr, "사용법: %s alice|bob\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *role = argv[1];
    const char *in_path;
    const char *out_path;

    if (strcmp(role, "alice") == 0) {
        in_path = FIFO_B2A;
        out_path = FIFO_A2B;
    } else {
        in_path = FIFO_A2B;
        out_path = FIFO_B2A;
    }

    make_fifo(FIFO_A2B);
    make_fifo(FIFO_B2A);

    /*
     * O_RDWR로 열면 상대방이 아직 FIFO를 열지 않아도 open()이 블록되지 않고,
     * 자기 자신이 writer로 남아 있어 read()가 EOF로 끝나지 않는다.
     */
    int in_fd = open(in_path, O_RDWR);
    if (in_fd == -1) {
        perror("open(in)");
        return EXIT_FAILURE;
    }

    int out_fd = open(out_path, O_RDWR);
    if (out_fd == -1) {
        perror("open(out)");
        close(in_fd);
        return EXIT_FAILURE;
    }

    /* 상대방이 종료되어 write()가 실패해도 SIGPIPE로 죽지 않게 한다. */
    signal(SIGPIPE, SIG_IGN);

    printf("[%s] 채팅을 시작합니다. 종료하려면 quit을 입력하세요.\n", role);
    fflush(stdout);

    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[1].fd = in_fd;
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

        /* 키보드 입력 → 상대방에게 전송 */
        if (fds[0].revents & POLLIN) {
            char line[256];
            if (fgets(line, sizeof(line), stdin) == NULL) {
                break;
            }

            if (strcmp(line, "quit\n") == 0 || strcmp(line, "quit") == 0) {
                break;
            }

            ssize_t written = write(out_fd, line, strlen(line));
            if (written == -1) {
                perror("write");
                break;
            }
        }

        /* 상대방이 보낸 메시지 → 화면에 출력 */
        if (fds[1].revents & (POLLIN | POLLHUP)) {
            char buffer[256];
            ssize_t count = read(in_fd, buffer, sizeof(buffer) - 1);
            if (count > 0) {
                buffer[count] = '\0';
                printf("상대: %s", buffer);
                fflush(stdout);
            } else if (count == 0) {
                printf("상대방이 채팅을 종료했습니다.\n");
                running = 0;
            } else if (errno != EINTR) {
                perror("read");
                running = 0;
            }
        }
    }

    close(in_fd);
    close(out_fd);

    /* 두 프로세스가 모두 unlink하므로 ENOENT는 무시한다. */
    unlink(FIFO_A2B);
    unlink(FIFO_B2A);

    printf("[%s] 채팅을 종료합니다.\n", role);
    return EXIT_SUCCESS;
}
