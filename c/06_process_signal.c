/*
 * 6번: 프로세스 및 시그널
 */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static volatile sig_atomic_t got_sigint = 0;
static volatile sig_atomic_t got_sigalrm = 0;

static void on_sigint(int signo) {
    (void)signo;
    got_sigint = 1;
    static const char message[] = "\n[SIGINT] Ctrl-C를 받았습니다.\n";
    (void)write(STDOUT_FILENO, message, sizeof(message) - 1);
}

static void on_sigusr1(int signo) {
    (void)signo;
    static const char message[] = "[SIGUSR1] 사용자 정의 시그널을 받았습니다.\n";
    (void)write(STDOUT_FILENO, message, sizeof(message) - 1);
}

static void on_sigalrm(int signo) {
    (void)signo;
    got_sigalrm = 1;
    static const char message[] = "[SIGALRM] alarm() 시간이 되었습니다.\n";
    (void)write(STDOUT_FILENO, message, sizeof(message) - 1);
}

static void install_handler(int signo, void (*handler)(int)) {
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);

    if (sigaction(signo, &action, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

static void wait_and_report(pid_t pid, const char *label) {
    int status;

    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return;
    }

    if (WIFEXITED(status)) {
        printf("%s 종료 코드: %d\n", label, WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("%s 시그널 종료: %d\n", label, WTERMSIG(status));
    }
}

int main(void) {
    printf("== 프로세스와 시그널 연습 ==\n");
    printf("현재 PID: %d, 부모 PID: %d\n\n", (int)getpid(), (int)getppid());
    fflush(stdout);

    /* 1. fork + waitpid */
    printf("[1] fork()로 자식 프로세스 생성\n");
    fflush(stdout);

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        printf("    자식 PID: %d, 부모 PID: %d\n", (int)getpid(), (int)getppid());
        printf("    자식은 1초 뒤에 종료 코드 7로 종료합니다.\n");
        fflush(stdout);
        sleep(1);
        _exit(7);
    }

    wait_and_report(pid, "    자식");
    printf("\n");

    /* 2. exec */
    printf("[2] fork() + execlp()로 ls -l 실행\n");
    fflush(stdout);

    pid = fork();
    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        execlp("ls", "ls", "-l", "--color=never", (char *)NULL);
        perror("execlp");
        _exit(127);
    }

    wait_and_report(pid, "    ls");
    printf("\n");

    /* 3. 시그널 처리기 등록 */
    install_handler(SIGINT, on_sigint);
    install_handler(SIGUSR1, on_sigusr1);
    install_handler(SIGALRM, on_sigalrm);

    printf("[3] SIGUSR1을 raise()로 보냅니다.\n");
    fflush(stdout);
    if (raise(SIGUSR1) != 0) {
        perror("raise");
    }
    printf("\n");

    printf("[4] alarm(2)를 설정하고 pause()로 기다립니다.\n");
    printf("    2초 안에 Ctrl-C를 눌러 보세요.\n");
    fflush(stdout);

    alarm(2);
    pause();
    alarm(0);

    printf("\n");
    if (got_sigint) {
        printf("SIGINT 처리 결과: Ctrl-C를 정상적으로 받았습니다.\n");
    }
    if (got_sigalrm) {
        printf("SIGALRM 처리 결과: alarm()이 정상적으로 동작했습니다.\n");
    }
    if (!got_sigint && !got_sigalrm) {
        printf("시그널을 받지 못했습니다.\n");
    }

    return EXIT_SUCCESS;
}
