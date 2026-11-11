# C 프로그램 (실습 순서 3~12번)

`c/` 디렉토리에 아래 이름으로 `.c` 파일을 만들고 `make`로 빌드합니다.
빌드 결과는 `c/bin/`에 생성되며 Git에는 올라가지 않습니다.

```bash
cd /mnt/c/Users/gong0000/Desktop/lab2/c
make                 # 모든 .c 파일 빌드
make bin/04_ls_r     # 하나만 빌드
./bin/04_ls_r .      # 실행
make clean           # 실행 파일 삭제
```

## 과제별 파일과 핵심 함수

| 번호 | 파일 | 핵심 내용 / 함수 |
|---|---|---|
| 3 | `03_file_dir.c` | `open`, `read`, `write`, `close`, `stat`, `opendir`, `readdir`, `closedir`, `mkdir`, `rmdir`, `unlink`, `rename` |
| 4 | `04_ls_r.c` | `opendir`/`readdir` 재귀 호출, `.`과 `..` 제외, 경로 조합 |
| 5 | `05_typing.c` | `clock_gettime`, 문자열 비교, 오타 수, 분당 타자수(CPM/WPM) 계산 |
| 6 | `06_process_signal.c` | `fork`, `exec`, `waitpid`, `signal`/`sigaction`, `kill`, `alarm`, `pause` |
| 7 | `07_ipc_practice.c` | `pipe`, `mkfifo`, `mq_open`, `shm_open`, `mmap`, `sem_open` 등 연습 |
| 8 | `08_pipe_chat.c` | 이름 있는 파이프(FIFO) 2개 또는 `fork` + `pipe` 2쌍, `select`/`poll` |
| 9 | `09_mq_chat.c` | `mq_open`, `mq_send`, `mq_receive`, `mq_close`, `mq_unlink` |
| 10 | `10_mmap_copy.c` | `open`, `fstat`, `ftruncate`, `mmap`, `memcpy`, `msync`, `munmap` |
| 11 | `11_dbus_server.c`, `11_dbus_client.c` | libdbus 또는 `dbus-send`/GDBus를 이용한 알림 메시지 |
| 12 | `12_shell.c` | `fork`, `execvp`, `waitpid`, `&`, `SIGINT`/`SIGQUIT`, `>`, `<`, `\|`, 내장 명령 |

> 9번, 11번처럼 라이브러리가 필요한 과제는 Makefile에 링크 옵션을 추가해야 할 수 있습니다.
> 예: `gcc ... -o bin/09_mq_chat 09_mq_chat.c -lrt`
> DBus: `pkg-config --cflags --libs dbus-1`

## 권장 진행 순서

1. 3번으로 파일/디렉토리 함수에 익숙해진다.
2. 4번에서 3번에서 배운 `opendir`/`readdir`을 재귀적으로 사용한다.
3. 5번은 파일 입출력과 시간 측정을 함께 연습한다.
4. 6~7번은 `fork`, `wait`, `signal`, `pipe`의 기본 동작을 먼저 확인한다.
5. 8~10번은 IPC를 실제 프로그램으로 완성한다.
6. 11번은 DBus, 12번은 지금까지 배운 `fork`/`exec`/`pipe`/시그널을 모두 합친다.
