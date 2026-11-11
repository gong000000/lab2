# C 프로그램 (실습 순서 3~11번)

`c/` 디렉토리에 과제별 C 소스가 있습니다. 빌드 결과는 `c/bin/`에 생성되며
Git에는 올라가지 않습니다.

```bash
cd /mnt/c/Users/gong0000/Desktop/lab2/c
make                 # 3~10번 + (DBus 패키지가 있으면) 11번 빌드
make bin/09_mq_chat  # 하나만 빌드
./bin/09_mq_chat --clean
make clean           # 실행 파일 삭제
```

## 구현 완료

| 번호 | 파일 | 핵심 내용 / 함수 | 실행 |
|---|---|---|---|
| 3 | `03_file_dir.c` | `open`, `read`, `write`, `close`, `stat`, `opendir`, `readdir`, `mkdir`, `rmdir`, `unlink`, `rename` | `./bin/03_file_dir` |
| 4 | `04_ls_r.c` | `scandir` + `alphasort`, 재귀 호출, `lstat`, `.`/`..` 제외 | `./bin/04_ls_r .` |
| 5 | `05_typing.c` | `clock_gettime`, 문자열 비교, 오타 수, CPM/WPM | `./bin/05_typing` |
| 6 | `06_process_signal.c` | `fork`, `execlp`, `waitpid`, `sigaction`, `raise`, `alarm`, `pause` | `./bin/06_process_signal` |
| 7 | `07_ipc_practice.c` | `pipe`, `mkfifo`, `shm_open`, `mmap`, `sem_open` | `./bin/07_ipc_practice` |
| 8 | `08_pipe_chat.c` | FIFO 2개, `poll`, `read`/`write` | 아래 참고 |
| 9 | `09_mq_chat.c` | `mq_open`, `mq_send`, `mq_receive`, `mq_close`, `mq_unlink` | 아래 참고 |
| 10 | `10_mmap_copy.c` | `open`, `fstat`, `ftruncate`, `mmap`, `memcpy`, `msync`, `munmap` | `./bin/10_mmap_copy 원본 대상` |
| 11 | `11_dbus_server.c`, `11_dbus_client.c` | libdbus 서버/클라이언트 | 아래 참고 |

12번 팀 과제(쉘)는 이번 범위에서 제외했습니다.

## 8번 — 파이프 채팅

터미널 2개를 열고 각각 실행합니다.

```bash
# 터미널 1
./bin/08_pipe_chat alice

# 터미널 2
./bin/08_pipe_chat bob
```

한쪽에서 `quit`을 입력하면 종료됩니다.

## 9번 — 메시지 큐 채팅

```bash
./bin/09_mq_chat --clean   # 이전 실행의 큐 정리 (처음 한 번)

# 터미널 1
./bin/09_mq_chat alice

# 터미널 2
./bin/09_mq_chat bob
```

`O_CREAT | O_RDWR`로 큐를 열고, `poll()`로 키보드 입력과 수신 큐를 동시에 감시합니다.
종료할 때 `mq_unlink()`로 큐를 정리합니다.

## 10번 — mmap 파일 복사

```bash
./bin/10_mmap_copy 원본.txt 복사본.txt
cmp 원본.txt 복사본.txt && echo "같음"
```

## 11번 — DBus 알림 서버/클라이언트

DBus 개발 패키지를 설치합니다.

```bash
sudo apt update
sudo apt install -y libdbus-1-dev pkg-config dbus
```

WSL에서 세션 버스가 없으면 `dbus-run-session`으로 같은 버스를 사용합니다.

```bash
dbus-run-session -- bash

# 같은 셸 안에서
./bin/11_dbus_server &
./bin/11_dbus_client "Hello from client"
```

서버 터미널에 `[알림] Hello from client`가 출력되면 성공입니다.

> `make`는 `pkg-config --exists dbus-1`이 성공할 때만 11번을 기본 빌드에 포함합니다.
> 11번이 빠졌다면 `make dbus`로 설치 안내를 확인하세요.
