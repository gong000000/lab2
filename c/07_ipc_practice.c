/*
 * 7번: IPC 함수 연습
 */
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define FIFO_PATH "/tmp/lab2_ipc_fifo"
#define SHARED_SIZE 256

struct shared_data {
    char message[SHARED_SIZE];
    int number;
};

static void fail(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

static void demo_pipe(void) {
    printf("[1] 익명 파이프(pipe)\n");

    int fd[2];
    if (pipe(fd) == -1) {
        fail("pipe");
    }

    fflush(stdout);
    pid_t pid = fork();
    if (pid == -1) {
        fail("fork");
    }

    if (pid == 0) {
        close(fd[1]);

        char buffer[128];
        ssize_t count = read(fd[0], buffer, sizeof(buffer) - 1);
        if (count == -1) {
            fail("read(pipe)");
        }
        buffer[count] = '\0';
        printf("    자식이 파이프에서 읽음: %s\n", buffer);
        fflush(stdout);

        close(fd[0]);
        _exit(EXIT_SUCCESS);
    }

    close(fd[0]);

    const char *message = "hello through pipe";
    if (write(fd[1], message, strlen(message)) == -1) {
        fail("write(pipe)");
    }

    close(fd[1]);
    if (waitpid(pid, NULL, 0) == -1) {
        fail("waitpid(pipe)");
    }
}

static void demo_fifo(void) {
    printf("[2] 이름 있는 파이프(FIFO)\n");

    unlink(FIFO_PATH); /* 이전 실행에서 남은 FIFO 정리 */
    if (mkfifo(FIFO_PATH, 0600) == -1) {
        fail("mkfifo");
    }

    fflush(stdout);
    pid_t pid = fork();
    if (pid == -1) {
        fail("fork");
    }

    if (pid == 0) {
        int fd = open(FIFO_PATH, O_RDONLY);
        if (fd == -1) {
            fail("open(FIFO read)");
        }

        char buffer[128];
        ssize_t count = read(fd, buffer, sizeof(buffer) - 1);
        if (count == -1) {
            fail("read(FIFO)");
        }
        buffer[count] = '\0';
        printf("    자식이 FIFO에서 읽음: %s\n", buffer);
        fflush(stdout);

        close(fd);
        _exit(EXIT_SUCCESS);
    }

    int fd = open(FIFO_PATH, O_WRONLY);
    if (fd == -1) {
        fail("open(FIFO write)");
    }

    const char *message = "hello through named pipe";
    if (write(fd, message, strlen(message)) == -1) {
        fail("write(FIFO)");
    }

    close(fd);
    if (waitpid(pid, NULL, 0) == -1) {
        fail("waitpid(FIFO)");
    }

    if (unlink(FIFO_PATH) == -1) {
        fail("unlink(FIFO)");
    }
}

static void demo_shared_memory(void) {
    printf("[3] POSIX 공유 메모리 + 세마포어\n");

    char shm_name[64];
    char sem_name[64];
    snprintf(shm_name, sizeof(shm_name), "/lab2_shm_%ld", (long)getpid());
    snprintf(sem_name, sizeof(sem_name), "/lab2_sem_%ld", (long)getpid());

    /* 이전 실행에서 남은 이름이 있으면 정리한다. */
    shm_unlink(shm_name);
    sem_unlink(sem_name);

    int fd = shm_open(shm_name, O_CREAT | O_RDWR, 0600);
    if (fd == -1) {
        fail("shm_open");
    }

    if (ftruncate(fd, sizeof(struct shared_data)) == -1) {
        fail("ftruncate");
    }

    struct shared_data *data = mmap(NULL, sizeof(struct shared_data),
                                    PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        fail("mmap");
    }
    close(fd);

    sem_t *semaphore = sem_open(sem_name, O_CREAT | O_EXCL, 0600, 0);
    if (semaphore == SEM_FAILED) {
        fail("sem_open");
    }

    fflush(stdout);
    pid_t pid = fork();
    if (pid == -1) {
        fail("fork");
    }

    if (pid == 0) {
        if (sem_wait(semaphore) == -1) {
            fail("sem_wait(child)");
        }

        printf("    자식이 공유 메모리에서 읽음: %s (number=%d)\n",
               data->message, data->number);
        fflush(stdout);

        if (sem_post(semaphore) == -1) {
            fail("sem_post(child)");
        }
        if (munmap(data, sizeof(struct shared_data)) == -1) {
            fail("munmap(child)");
        }
        _exit(EXIT_SUCCESS);
    }

    snprintf(data->message, sizeof(data->message), "hello through shared memory");
    data->number = 42;

    if (sem_post(semaphore) == -1) {
        fail("sem_post(parent)");
    }

    if (waitpid(pid, NULL, 0) == -1) {
        fail("waitpid(shm)");
    }

    if (munmap(data, sizeof(struct shared_data)) == -1) {
        fail("munmap(parent)");
    }
    if (sem_close(semaphore) == -1) {
        fail("sem_close");
    }
    if (sem_unlink(sem_name) == -1) {
        fail("sem_unlink");
    }
    if (shm_unlink(shm_name) == -1) {
        fail("shm_unlink");
    }
}

int main(void) {
    printf("== IPC 함수 연습 ==\n\n");

    demo_pipe();
    printf("\n");
    demo_fifo();
    printf("\n");
    demo_shared_memory();

    printf("\n모든 IPC 연습 완료\n");
    return EXIT_SUCCESS;
}
