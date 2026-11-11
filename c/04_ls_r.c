/*
 * 4번: 디렉토리 재귀 탐색 (ls -R)
 */
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static void list_recursive(const char *path) {
    char base[PATH_MAX];
    snprintf(base, sizeof(base), "%s", path);

    /* 끝에 붙은 '/'를 제거한다. 단, 루트("/")는 그대로 둔다. */
    size_t length = strlen(base);
    while (length > 1 && base[length - 1] == '/') {
        base[--length] = '\0';
    }

    /*
     * scandir + alphasort를 사용하면 ls처럼 이름순으로 정렬된다.
     * 반환값은 항목 개수이고, entries[i]는 각각 free해야 한다.
     */
    struct dirent **entries;
    int count = scandir(base, &entries, NULL, alphasort);
    if (count < 0) {
        perror(base);
        return;
    }

    printf("%s:\n", base);

    /* 현재 디렉토리의 항목을 먼저 모두 출력한다. */
    for (int i = 0; i < count; i++) {
        if (strcmp(entries[i]->d_name, ".") == 0 ||
            strcmp(entries[i]->d_name, "..") == 0) {
            continue;
        }
        printf("%s\n", entries[i]->d_name);
    }

    /* 그 다음 하위 디렉토리만 재귀적으로 방문한다. */
    for (int i = 0; i < count; i++) {
        if (strcmp(entries[i]->d_name, ".") == 0 ||
            strcmp(entries[i]->d_name, "..") == 0) {
            continue;
        }

        char full_path[PATH_MAX];
        int written = snprintf(full_path, sizeof(full_path), "%s/%s",
                               base, entries[i]->d_name);
        if (written < 0 || (size_t)written >= sizeof(full_path)) {
            fprintf(stderr, "경로가 너무 깁니다: %s/%s\n", base, entries[i]->d_name);
            continue;
        }

        struct stat info;
        /*
         * lstat을 사용하면 심볼릭 링크를 따라가지 않는다.
         * 따라서 링크가 디렉토리를 가리켜도 무한 재귀에 빠지지 않는다.
         */
        if (lstat(full_path, &info) == -1) {
            perror(full_path);
            continue;
        }

        if (S_ISDIR(info.st_mode)) {
            printf("\n");
            list_recursive(full_path);
        }
    }

    for (int i = 0; i < count; i++) {
        free(entries[i]);
    }
    free(entries);
}

int main(int argc, char *argv[]) {
    const char *path = (argc > 1) ? argv[1] : ".";

    if (argc > 2) {
        fprintf(stderr, "사용법: %s [디렉토리]\n", argv[0]);
        return EXIT_FAILURE;
    }

    list_recursive(path);
    return 0;
}
