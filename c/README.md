# C Programs

2번은 Rust라서 `rust/matrix_add`에 있습니다. 이 디렉토리는 3~11번 C 프로그램입니다.

## Build

```bash
cd c
make
make clean
```

11번 DBus는 `libdbus-1-dev`, `pkg-config`, `dbus`가 필요합니다.

```bash
sudo apt install -y libdbus-1-dev pkg-config dbus
```

## Files

| File | Description |
|---|---|
| `03_file_dir.c` | 파일 및 디렉토리 함수 |
| `04_ls_r.c` | 디렉토리 재귀 탐색 (`ls -R`) |
| `05_typing.c` | 타자 연습 |
| `06_process_signal.c` | 프로세스 및 시그널 |
| `07_ipc_practice.c` | IPC 함수 연습 |
| `08_pipe_chat.c` | 파이프 채팅 |
| `09_mq_chat.c` | POSIX 메시지 큐 채팅 |
| `10_mmap_copy.c` | mmap 파일 복사 |
| `11_dbus_server.c` | DBus 알림 서버 |
| `11_dbus_client.c` | DBus 알림 클라이언트 |

## Run

```bash
./bin/03_file_dir
./bin/04_ls_r .
./bin/05_typing
./bin/06_process_signal
./bin/07_ipc_practice
./bin/10_mmap_copy source.txt copy.txt
```

### 8번

```bash
./bin/08_pipe_chat alice
./bin/08_pipe_chat bob
```

### 9번

```bash
./bin/09_mq_chat --clean
./bin/09_mq_chat alice
./bin/09_mq_chat bob
```

### 11번

```bash
dbus-run-session -- bash
./bin/11_dbus_server &
./bin/11_dbus_client "Hello"
```
