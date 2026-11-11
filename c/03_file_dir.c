/*
 * 3번: 파일 및 디렉토리 관련 함수
 */
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static void fail(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int main(void) {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        fail("getcwd");
    }
    printf("현재 디렉토리: %s\n\n", cwd);

    /*
     * 실행할 때마다 이름이 겹치지 않도록 PID를 붙인다.
     * 예: lab2_test_dir_12345
     */
    char dir_name[64];
    snprintf(dir_name, sizeof(dir_name), "lab2_test_dir_%ld", (long)getpid());

    char file_name[PATH_MAX];
    char renamed_name[PATH_MAX];
    snprintf(file_name, sizeof(file_name), "%s/hello.txt", dir_name);
    snprintf(renamed_name, sizeof(renamed_name), "%s/hello_renamed.txt", dir_name);

    /* 1. 디렉토리 생성 */
    if (mkdir(dir_name, 0755) == -1) {
        fail("mkdir");
    }
    printf("[1] mkdir   : %s 디렉토리 생성\n", dir_name);

    /* 2. 파일 열기 + 쓰기 */
    int fd = open(file_name, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
        fail("open(O_WRONLY)");
    }

    const char *text = "Hello, Linux file I/O!\n";
    ssize_t written = write(fd, text, strlen(text));
    if (written == -1) {
        fail("write");
    }
    if (close(fd) == -1) {
        fail("close");
    }
    printf("[2] write   : %s에 %zd바이트 기록\n", file_name, written);

    /* 3. 파일 정보 확인 */
    struct stat info;
    if (stat(file_name, &info) == -1) {
        fail("stat");
    }
    printf("[3] stat    : 크기=%lld바이트, 권한=%04o, inode=%llu\n",
           (long long)info.st_size,
           info.st_mode & 07777,
           (unsigned long long)info.st_ino);

    /* 4. 파일 열기 + 읽기 */
    fd = open(file_name, O_RDONLY);
    if (fd == -1) {
        fail("open(O_RDONLY)");
    }

    char buffer[256];
    ssize_t read_count = read(fd, buffer, sizeof(buffer) - 1);
    if (read_count == -1) {
        fail("read");
    }
    buffer[read_count] = '\0';
    if (close(fd) == -1) {
        fail("close");
    }
    printf("[4] read    : %s", buffer);

    /* 5. 디렉토리 열기 + 내용 나열 */
    DIR *dir = opendir(dir_name);
    if (dir == NULL) {
        fail("opendir");
    }

    printf("[5] readdir : %s 내용\n", dir_name);
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        printf("             %s\n", entry->d_name);
    }
    if (closedir(dir) == -1) {
        fail("closedir");
    }

    /* 6. 파일 이름 변경 */
    if (rename(file_name, renamed_name) == -1) {
        fail("rename");
    }
    printf("[6] rename  : %s -> %s\n", file_name, renamed_name);

    /* 7. 파일 삭제 + 디렉토리 삭제 */
    if (unlink(renamed_name) == -1) {
        fail("unlink");
    }
    if (rmdir(dir_name) == -1) {
        fail("rmdir");
    }
    printf("[7] unlink  : 파일과 디렉토리 삭제 완료\n");

    return 0;
}
