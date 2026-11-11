CC      := gcc
CFLAGS  := -Wall -Wextra -Werror -g -D_GNU_SOURCE
BIN_DIR := bin

# c/ 디렉토리의 모든 .c 파일을 bin/ 아래 실행 파일로 만든다.
SOURCES := $(wildcard *.c)
TARGETS := $(patsubst %.c,$(BIN_DIR)/%,$(SOURCES))

.PHONY: all clean list

all: $(TARGETS)

$(BIN_DIR)/%: %.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

list:
	@echo "소스 파일:"
	@ls -1 *.c 2>/dev/null || echo "  (없음)"
	@echo "실행 파일:"
	@ls -1 $(BIN_DIR) 2>/dev/null || echo "  (없음)"

clean:
	rm -rf $(BIN_DIR)
