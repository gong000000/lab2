CC      := gcc
CFLAGS  := -Wall -Wextra -Werror -g -D_GNU_SOURCE -pthread
LDLIBS  := -pthread -lrt
BIN_DIR := bin

# DBus 개발 패키지(libdbus-1-dev, pkg-config)가 있으면 11번도 같이 빌드한다.
DBUS_CFLAGS := $(shell pkg-config --cflags dbus-1 2>/dev/null)
DBUS_LIBS   := $(shell pkg-config --libs dbus-1 2>/dev/null)

DBUS_SOURCES := 11_dbus_server.c 11_dbus_client.c
C_SOURCES    := $(filter-out $(DBUS_SOURCES),$(wildcard *.c))
TARGETS      := $(patsubst %.c,$(BIN_DIR)/%,$(C_SOURCES))

ifneq ($(strip $(DBUS_LIBS)),)
TARGETS += $(BIN_DIR)/11_dbus_server $(BIN_DIR)/11_dbus_client
endif

.PHONY: all clean list dbus

all: $(TARGETS)

$(BIN_DIR)/%: %.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

$(BIN_DIR)/11_dbus_server: 11_dbus_server.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $(DBUS_CFLAGS) $< -o $@ $(DBUS_LIBS) $(LDLIBS)

$(BIN_DIR)/11_dbus_client: 11_dbus_client.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $(DBUS_CFLAGS) $< -o $@ $(DBUS_LIBS) $(LDLIBS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

list:
	@echo "소스 파일:"
	@ls -1 *.c 2>/dev/null || echo "  (없음)"
	@echo "실행 파일:"
	@ls -1 $(BIN_DIR) 2>/dev/null || echo "  (없음)"
	@echo "DBus 빌드: $(if $(strip $(DBUS_LIBS)),가능,불가능 - sudo apt install -y libdbus-1-dev pkg-config)"

dbus:
	@echo "11번 DBus 빌드에는 libdbus-1-dev와 pkg-config가 필요합니다."
	@echo "  sudo apt install -y libdbus-1-dev pkg-config"
	@echo "  make bin/11_dbus_server bin/11_dbus_client"

clean:
	rm -rf $(BIN_DIR)
