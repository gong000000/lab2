/*
 * 5번: 타자 연습
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_INPUT 512

static const char *const SENTENCES[] = {
    "The quick brown fox jumps over the lazy dog.",
    "Linux is a free and open source operating system.",
    "Practice makes perfect when learning programming.",
    "A journey of a thousand miles begins with a single step.",
    "Rust and C are both systems programming languages."
};

static const size_t SENTENCE_COUNT = sizeof(SENTENCES) / sizeof(SENTENCES[0]);

static void strip_newline(char *text) {
    size_t length = strlen(text);
    while (length > 0 && (text[length - 1] == '\n' || text[length - 1] == '\r')) {
        text[--length] = '\0';
    }
}

/*
 * 두 문자열을 비교해서 틀린 글자 수를 반환한다.
 * 길이가 다르면 남는 글자도 오타로 센다.
 */
static size_t count_errors(const char *target, const char *input) {
    size_t target_length = strlen(target);
    size_t input_length = strlen(input);
    size_t errors = 0;

    size_t common = target_length < input_length ? target_length : input_length;
    for (size_t i = 0; i < common; i++) {
        if (target[i] != input[i]) {
            errors++;
        }
    }

    if (target_length > input_length) {
        errors += target_length - input_length;
    } else if (input_length > target_length) {
        errors += input_length - target_length;
    }

    return errors;
}

int main(void) {
    size_t total_errors = 0;
    size_t total_typed = 0;
    double total_seconds = 0.0;

    printf("== 타자 연습 ==\n");
    printf("화면에 나오는 문장을 그대로 입력하세요. (총 %zu문장)\n\n", SENTENCE_COUNT);

    for (size_t i = 0; i < SENTENCE_COUNT; i++) {
        const char *target = SENTENCES[i];

        printf("[%zu/%zu] 다음 문장을 입력하세요:\n", i + 1, SENTENCE_COUNT);
        printf("  %s\n", target);
        printf("> ");
        fflush(stdout);

        char input[MAX_INPUT];
        struct timespec start;
        struct timespec end;

        if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
            perror("clock_gettime");
            return EXIT_FAILURE;
        }

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n입력이 끝났습니다.\n");
            break;
        }

        if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
            perror("clock_gettime");
            return EXIT_FAILURE;
        }

        strip_newline(input);

        double seconds = (double)(end.tv_sec - start.tv_sec)
                       + (double)(end.tv_nsec - start.tv_nsec) / 1e9;
        if (seconds <= 0.0) {
            seconds = 0.001; /* 0으로 나누는 것을 방지 */
        }

        size_t errors = count_errors(target, input);
        size_t typed = strlen(input);
        double cpm = (double)typed * 60.0 / seconds;
        double wpm = cpm / 5.0; /* 영어 기준: 5글자를 1단어로 계산 */

        total_errors += errors;
        total_typed += typed;
        total_seconds += seconds;

        printf("  오타: %zu개, 입력 글자: %zu개, 시간: %.2f초\n",
               errors, typed, seconds);
        printf("  CPM: %.1f, WPM: %.1f\n\n", cpm, wpm);
    }

    if (total_seconds > 0.0 && total_typed > 0) {
        double average_cpm = (double)total_typed * 60.0 / total_seconds;
        double average_wpm = average_cpm / 5.0;

        printf("== 전체 결과 ==\n");
        printf("총 오타: %zu개\n", total_errors);
        printf("총 입력 글자: %zu개\n", total_typed);
        printf("총 시간: %.2f초\n", total_seconds);
        printf("평균 CPM: %.1f\n", average_cpm);
        printf("평균 WPM: %.1f\n", average_wpm);
    }

    return EXIT_SUCCESS;
}
