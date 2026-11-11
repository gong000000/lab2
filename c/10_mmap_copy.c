/*
 * 10번 과제: POSIX 공유 메모리/mmap을 사용한 파일 복사
 *
 * 사용법:
 *   ./bin/10_mmap_copy 원본파일 대상파일
 *
 * 원본 파일을 MAP_PRIVATE로, 대상 파일을 MAP_SHARED로 매핑한 뒤
 * memcpy()로 내용을 복사하고 msync()로 디스크에 반영한다.
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

static void fail(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "사용법: %s <원본파일> <대상파일>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *source_path = argv[1];
    const char *target_path = argv[2];

    int source_fd = open(source_path, O_RDONLY);
    if (source_fd == -1) {
        fail("open(source)");
    }

    struct stat info;
    if (fstat(source_fd, &info) == -1) {
        fail("fstat");
    }

    if (info.st_size < 0) {
        fprintf(stderr, "파일 크기가 올바르지 않습니다.\n");
        close(source_fd);
        return EXIT_FAILURE;
    }

    size_t size = (size_t)info.st_size;

    int target_fd = open(target_path, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (target_fd == -1) {
        fail("open(target)");
    }

    if (ftruncate(target_fd, info.st_size) == -1) {
        fail("ftruncate");
    }

    if (size == 0) {
        /* 길이가 0이면 mmap()을 호출할 수 없으므로 빈 파일만 만든다. */
        close(source_fd);
        close(target_fd);
        printf("복사 완료: %s -> %s (0 bytes)\n", source_path, target_path);
        return EXIT_SUCCESS;
    }

    void *source_map = mmap(NULL, size, PROT_READ, MAP_PRIVATE, source_fd, 0);
    if (source_map == MAP_FAILED) {
        fail("mmap(source)");
    }

    void *target_map = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, target_fd, 0);
    if (target_map == MAP_FAILED) {
        munmap(source_map, size);
        fail("mmap(target)");
    }

    memcpy(target_map, source_map, size);

    if (msync(target_map, size, MS_SYNC) == -1) {
        fail("msync");
    }

    if (munmap(source_map, size) == -1) {
        fail("munmap(source)");
    }
    if (munmap(target_map, size) == -1) {
        fail("munmap(target)");
    }

    if (close(source_fd) == -1) {
        fail("close(source)");
    }
    if (close(target_fd) == -1) {
        fail("close(target)");
    }

    printf("복사 완료: %s -> %s (%zu bytes)\n", source_path, target_path, size);
    return EXIT_SUCCESS;
}
