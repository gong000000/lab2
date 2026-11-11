# Lab2 실습 순서별 가이드 (5장)

이 문서는 `Lab2.pdf`의 **5. 실습 순서** 2~12번을 진행하기 위한 요약 가이드입니다.
2~4번은 이미 구현되어 있고, 5번부터 이 문서를 보면서 하나씩 추가하면 됩니다.

## 공통 작업 순서

### 1) WSL 터미널에서 빌드/실행

```bash
wsl
cd /mnt/c/Users/gong0000/Desktop/lab2
```

C 프로그램:

```bash
cd c
make                 # c/*.c 전체 빌드
make bin/05_typing   # 하나만 빌드
./bin/05_typing      # 실행
```

Rust 프로그램:

```bash
cd rust/matrix_add
cargo run
```

### 2) Windows PowerShell에서 Git 커밋/푸시

```powershell
cd C:\Users\gong0000\Desktop\lab2
git status
git add .
git commit -m "feat: add typing practice"
git push
```

> WSL의 git에는 GitHub 로그인 정보가 없을 수 있으므로 push는 PowerShell에서 하는 것을 권장합니다.

### 3) 과제를 하나 끝낼 때마다

1. 실행 결과를 캡처한다.
2. `git add → commit → push` 한다.
3. README의 체크리스트를 `[x]`로 바꾼다.
4. 보고서에 코드 설명과 실행 결과를 정리한다.

---

## 5번 — 타자 연습 프로그램

**파일:** `c/05_typing.c`

**목표:** 문장을 보여 주고 사용자가 따라 치게 한 뒤, 틀린 횟수와 평균 분당 타자수를 계산한다.

**구현 순서**

1. 연습할 문장 배열을 만든다. (영어 문장이 바이트 수 계산에 편하다.)
2. 문장을 출력하고 `fgets`로 입력을 받는다.
3. 입력 전후에 `clock_gettime(CLOCK_MONOTONIC, ...)`을 호출해 걸린 시간을 잰다.
4. 원본 문장과 입력을 한 글자씩 비교해 오타 수를 센다.
5. `CPM = 전체 글자 수 × 60 / 걸린 초`, `WPM = CPM / 5`로 계산한다.
6. 문장별 결과와 전체 평균을 출력한다.

**시간 측정 예**

```c
struct timespec start, end;
clock_gettime(CLOCK_MONOTONIC, &start);

fgets(input, sizeof(input), stdin);

clock_gettime(CLOCK_MONOTONIC, &end);
double seconds = (end.tv_sec - start.tv_sec)
               + (end.tv_nsec - start.tv_nsec) / 1e9;
```

**오타 계산 예**

```c
int errors = 0;
size_t target_len = strlen(target);
size_t input_len = strlen(input);

for (size_t i = 0; i < target_len && i < input_len; i++) {
    if (target[i] != input[i]) errors++;
}
if (target_len > input_len) errors += target_len - input_len;
if (input_len > target_len) errors += input_len - target_len;
```

**확인**

```bash
cd /mnt/c/Users/gong0000/Desktop/lab2/c
make bin/05_typing
./bin/05_typing
```

일부러 틀리게 입력해서 오타 수가 증가하는지, 빨리 입력할수록 CPM이 커지는지 확인한다.

---

## 6번 — 프로세스와 시그널 연습

**파일:** `c/06_process_signal.c`

**목표:** `fork`, `wait`, `exec`, 시그널을 한 프로그램에서 연습한다.

**구현 순서**

1. `getpid()`, `getppid()`로 현재 프로세스 정보를 출력한다.
2. `fork()`로 자식 프로세스를 만들고, 자식은 `sleep(1)` 후 `_exit(7)`로 종료한다.
3. 부모는 `waitpid()`로 자식을 기다리고 `WIFEXITED`, `WEXITSTATUS`로 종료 코드를 확인한다.
4. 다른 자식에서 `execlp("ls", "ls", "-l", NULL)`을 실행해 `exec`를 연습한다.
5. `sigaction(SIGINT, ...)`로 Ctrl-C 처리기를 등록하고, 처리기가 호출되면 메시지를 출력하게 한다.
6. `alarm(3)`으로 SIGALRM을 발생시켜 처리기를 확인한다.

**fork/wait 예**

```c
pid_t pid = fork();
if (pid == 0) {
    printf("자식 PID: %d\n", getpid());
    sleep(1);
    _exit(7);
} else if (pid > 0) {
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        printf("자식 종료 코드: %d\n", WEXITSTATUS(status));
    }
} else {
    perror("fork");
}
```

**시그널 처리기 예**

```c
static volatile sig_atomic_t got_signal = 0;

static void on_sigint(int signo) {
    (void)signo;
    got_signal = 1;
    const char message[] = "\n[SIGINT] Ctrl-C를 받았습니다.\n";
    write(STDOUT_FILENO, message, sizeof(message) - 1);
}

struct sigaction action = {0};
action.sa_handler = on_sigint;
sigemptyset(&action.sa_mask);
sigaction(SIGINT, &action, NULL);
```

> 시그널 처리기 안에서는 `printf` 대신 `write`를 사용하는 것이 안전하다.

**확인**

```bash
make bin/06_process_signal
./bin/06_process_signal
```

실행 중 Ctrl-C를 눌러 처리기가 동작하는지 확인한다.

---

## 7번 — 프로세스 간 통신(IPC) 연습

**파일:** `c/07_ipc_practice.c`

**목표:** 8~10번에서 사용할 IPC 기법을 미리 연습한다. 하나의 프로그램에 모두 넣어도 되고, 여러 개로 나눠도 된다.

**연습할 것**

- `pipe()` + `fork()`: 부모가 쓰고 자식이 읽기
- `mkfifo()`: 이름 있는 파이프 생성
- `shm_open()` + `ftruncate()` + `mmap()`: POSIX 공유 메모리
- `sem_open()` / `sem_wait()` / `sem_post()`: 공유 자원 보호
- (선택) System V `msgget()` / `msgsnd()` / `msgrcv()`

**pipe 예**

```c
int fd[2];
pipe(fd);

pid_t pid = fork();
if (pid == 0) {
    close(fd[1]);
    char buffer[64];
    ssize_t n = read(fd[0], buffer, sizeof(buffer) - 1);
    if (n > 0) {
        buffer[n] = '\0';
        printf("자식이 받은 메시지: %s\n", buffer);
    }
    close(fd[0]);
    _exit(0);
}

close(fd[0]);
write(fd[1], "hello", 5);
close(fd[1]);
wait(NULL);
```

**확인**

```bash
make bin/07_ipc_practice
./bin/07_ipc_practice
ipcs -q      # 메시지 큐 확인
ipcs -m      # 공유 메모리 확인
```

---

## 8번 — 파이프 기반 채팅

**파일:** `c/08_pipe_chat.c`

**목표:** 두 터미널에서 서로 메시지를 주고받는 텍스트 채팅을 만든다.

**방법: 이름 있는 파이프(FIFO) 2개 사용**

- Alice → Bob: `/tmp/lab2_a2b`
- Bob → Alice: `/tmp/lab2_b2a`

```bash
mkfifo /tmp/lab2_a2b
mkfifo /tmp/lab2_b2a
```

터미널 1:

```bash
./bin/08_pipe_chat alice
```

터미널 2:

```bash
./bin/08_pipe_chat bob
```

**구현 순서**

1. `argv[1]`로 `alice` / `bob`을 구분한다.
2. `mkfifo()`로 두 FIFO를 만든다. 이미 있으면 `EEXIST`는 무시한다.
3. 들어오는 FIFO와 나가는 FIFO를 `open()`한다. 양쪽이 동시에 열 수 있도록 `O_RDWR`을 사용하면 편하다.
4. `poll()`로 `stdin`과 들어오는 FIFO를 동시에 감시한다.
5. `stdin`에 입력이 있으면 나가는 FIFO에 `write()`한다.
6. 들어오는 FIFO에 데이터가 있으면 `read()`해서 화면에 출력한다.
7. `quit`을 입력하면 종료하고 FIFO를 `unlink()`한다.

**poll 예**

```c
struct pollfd fds[2];
fds[0].fd = STDIN_FILENO;
fds[0].events = POLLIN;
fds[1].fd = in_fd;
fds[1].events = POLLIN;

while (1) {
    poll(fds, 2, -1);

    if (fds[0].revents & POLLIN) {
        char line[256];
        if (fgets(line, sizeof(line), stdin) == NULL) break;
        if (strcmp(line, "quit\n") == 0) break;
        write(out_fd, line, strlen(line));
    }

    if (fds[1].revents & POLLIN) {
        char buffer[256];
        ssize_t n = read(in_fd, buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0';
            printf("상대: %s", buffer);
            fflush(stdout);
        }
    }
}
```

**확인**

두 터미널에서 서로 메시지가 보이면 성공. 한쪽에서 `quit`을 입력해 종료한다.

---

## 9번 — POSIX 메시지 큐 기반 채팅

**파일:** `c/09_mq_chat.c`

**목표:** 8번과 같은 채팅을 POSIX 메시지 큐로 구현한다.

**큐 이름**

- Alice → Bob: `/lab2_a2b`
- Bob → Alice: `/lab2_b2a`

> POSIX 메시지 큐 이름은 반드시 `/`로 시작해야 한다.

**구현 순서**

1. `struct mq_attr`에 `mq_maxmsg = 10`, `mq_msgsize = 256`을 설정한다.
2. `mq_open(name, O_CREAT | O_RDWR, 0644, &attr)`로 큐를 연다.
3. 시작할 때 `mq_unlink()`로 이전 실행의 큐를 정리한다. (`ENOENT`는 무시)
4. `poll()`로 `stdin`과 들어오는 큐를 감시한다.
5. 입력이 있으면 `mq_send(outq, line, strlen(line) + 1, 1)`로 보낸다.
6. 큐에 메시지가 있으면 `mq_receive(inq, buffer, sizeof(buffer), NULL)`로 받아 출력한다.
7. 종료할 때 `mq_close()`와 `mq_unlink()`를 호출한다.

**핵심 코드**

```c
struct mq_attr attr = {0};
attr.mq_maxmsg = 10;
attr.mq_msgsize = 256;

mqd_t inq = mq_open(in_name, O_CREAT | O_RDWR, 0644, &attr);
mqd_t outq = mq_open(out_name, O_CREAT | O_RDWR, 0644, &attr);

mq_send(outq, line, strlen(line) + 1, 1);

char buffer[256];
ssize_t n = mq_receive(inq, buffer, sizeof(buffer), NULL);
if (n >= 0) {
    printf("상대: %s", buffer);
    fflush(stdout);
}
```

**컴파일**

최신 glibc에서는 `-lrt`가 없어도 되지만, 환경에 따라 필요할 수 있다.

```bash
gcc -Wall -Wextra -g -D_GNU_SOURCE 09_mq_chat.c -o bin/09_mq_chat -lrt
```

**확인**

```bash
./bin/09_mq_chat alice
./bin/09_mq_chat bob
ls /dev/mqueue
```

---

## 10번 — mmap을 이용한 파일 복사

**파일:** `c/10_mmap_copy.c`

**목표:** `mmap()`으로 원본 파일과 대상 파일을 메모리에 매핑하고 `memcpy()`로 복사한다.

**구현 순서**

1. 인자로 원본 파일과 대상 파일 경로를 받는다.
2. `open(src, O_RDONLY)`로 원본을 연다.
3. `fstat()`으로 원본 크기를 구한다.
4. `open(dst, O_RDWR | O_CREAT | O_TRUNC, 0644)`로 대상을 열고 `ftruncate()`로 크기를 맞춘다.
5. `mmap(NULL, size, PROT_READ, MAP_PRIVATE, src, 0)`로 원본을 매핑한다.
6. `mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, dst, 0)`로 대상을 매핑한다.
7. `memcpy(dst_map, src_map, size)`로 복사한다.
8. `msync(dst_map, size, MS_SYNC)`로 디스크에 반영한다.
9. `munmap()`, `close()`로 정리한다.

**주의**

- 크기가 0인 파일은 `mmap()` 길이 0으로 호출하면 실패하므로 별도로 처리한다.
- `mmap()` 반환값이 `MAP_FAILED`인지 반드시 확인한다.
- `msync()`를 생략하면 복사가 늦게 반영될 수 있다.

**확인**

```bash
make bin/10_mmap_copy
./bin/10_mmap_copy 원본.txt 복사본.txt
cmp 원본.txt 복사본.txt && echo "같음"
sha256sum 원본.txt 복사본.txt
```

---

## 11번 — DBus 알림 서버/클라이언트

**파일:** `c/11_dbus_server.c`, `c/11_dbus_client.c`

**목표:** DBus 버스에 이름을 등록한 서버가 클라이언트의 메시지를 받아 알림을 출력한다.

**준비**

```bash
sudo apt update
sudo apt install -y libdbus-1-dev pkg-config dbus
```

WSL에서 세션 버스가 없으면 `dbus-run-session`으로 두 프로그램을 같은 버스에서 실행한다.

```bash
dbus-run-session -- bash
# 이 셸 안에서 서버와 클라이언트를 각각 실행
```

**서버**

- `dbus_bus_get(DBUS_BUS_SESSION, &error)`로 세션 버스에 연결
- `dbus_bus_request_name(conn, "org.example.Lab2Notify", ...)`로 이름 등록
- `/org/example/Lab2Notify` 객체 경로와 `org.example.Lab2Notify` 인터페이스 등록
- `Send` 메서드에서 문자열 인자를 받아 출력
- `dbus_connection_read_write_dispatch(conn, -1)`로 메시지 대기

**클라이언트**

- 세션 버스에 연결
- `dbus_message_new_method_call("org.example.Lab2Notify", "/org/example/Lab2Notify", "org.example.Lab2Notify", "Send")`
- `dbus_message_append_args(msg, DBUS_TYPE_STRING, &text, DBUS_TYPE_INVALID)`
- `dbus_connection_send_with_reply_and_block(conn, msg, -1, &error)`

**컴파일**

```bash
gcc -Wall -Wextra -g $(pkg-config --cflags dbus-1) \
    11_dbus_server.c -o bin/11_dbus_server $(pkg-config --libs dbus-1)

gcc -Wall -Wextra -g $(pkg-config --cflags dbus-1) \
    11_dbus_client.c -o bin/11_dbus_client $(pkg-config --libs dbus-1)
```

**확인**

터미널 1에서 서버 실행, 터미널 2에서 클라이언트 실행.

```bash
./bin/11_dbus_server
./bin/11_dbus_client "Hello from client"
```

서버 터미널에 메시지가 출력되면 성공.

> WSL에는 데스크톱 알림 데몬이 없을 수 있다. 이 경우 “알림 메시지를 받아 화면에 출력하는 서버”로 구현해도 과제의 서버-클라이언트 구조를 확인할 수 있다.

---

## 12번 — 간단한 쉘 프로그램 (팀 과제)

**파일:** `c/12_shell.c`

**목표:** `fork`/`exec`/`wait`를 이용해 다음 기능을 가진 쉘을 만든다.

1. `exit` 입력 시 종료
2. 명령 끝의 `&`로 백그라운드 실행
3. Ctrl-C(SIGINT), Ctrl-Z(SIGQUIT) 처리
4. 파일 재지향 `>`, `<`와 파이프 `|`
5. `ls`, `pwd`, `cd`, `mkdir`, `rmdir`, `ln`, `cp`, `rm`, `mv`, `cat`, `grep` 구현

**권장 구현 순서**

1. 프롬프트 출력 → `fgets` → `fork` → `execvp` → `waitpid`
2. `exit`를 내장 명령으로 처리
3. `cd`, `pwd`를 내장 명령으로 처리 (`cd`는 반드시 쉘 자신이 실행)
4. `&` 백그라운드 실행
5. SIGINT/SIGQUIT 처리
6. `>` / `<` 재지향
7. `|` 파이프
8. 나머지 명령 구현

**기본 뼈대**

```c
while (1) {
    printf("lab2-shell> ");
    fflush(stdout);

    if (fgets(line, sizeof(line), stdin) == NULL) break;

    if (strcmp(line, "exit\n") == 0) break;

    /* TODO: &, 재지향, 파이프 파싱 */

    pid_t pid = fork();
    if (pid == 0) {
        /* 자식은 기본 시그널 동작으로 되돌린다. */
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);

        /* TODO: dup2()로 재지향/파이프 연결 */

        execvp(args[0], args);
        perror("execvp");
        _exit(127);
    } else if (pid > 0) {
        if (!background) {
            waitpid(pid, &status, 0);
        }
    } else {
        perror("fork");
    }
}
```

**시그널 처리 핵심**

- 쉘 자신은 `SIGINT`와 `SIGQUIT`를 무시한다.
- `fork()` 직후 자식은 `SIG_DFL`로 되돌린다.
- 그래야 Ctrl-C가 쉘은 죽이지 않고, 실행 중인 자식만 종료한다.

**재지향 핵심**

```c
int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
dup2(fd, STDOUT_FILENO);
close(fd);
```

**파이프 핵심**

```c
int fd[2];
pipe(fd);

pid_t left = fork();
if (left == 0) {
    dup2(fd[1], STDOUT_FILENO);
    close(fd[0]);
    close(fd[1]);
    execvp(left_args[0], left_args);
    _exit(127);
}

pid_t right = fork();
if (right == 0) {
    dup2(fd[0], STDIN_FILENO);
    close(fd[0]);
    close(fd[1]);
    execvp(right_args[0], right_args);
    _exit(127);
}

close(fd[0]);
close(fd[1]);
waitpid(left, NULL, 0);
waitpid(right, NULL, 0);
```

**테스트 예**

```text
lab2-shell> pwd
lab2-shell> ls -l
lab2-shell> sleep 5 &
lab2-shell> echo hello > out.txt
lab2-shell> cat < out.txt
lab2-shell> ls | grep txt
lab2-shell> ^C
lab2-shell> exit
```

**팀원 분담 예**

- A: 기본 루프, `exit`, `fork`/`exec`/`wait`
- B: `cd`, `pwd`, `mkdir`, `rmdir`
- C: `ls`, `cat`, `grep`
- D: `cp`, `rm`, `mv`, `ln`
- E: `&`, SIGINT/SIGQUIT
- F: `>`, `<`, `|` 파싱과 `dup2`

---

## 6장 보고서(검토) 작성 팁

각 과제마다 아래 내용을 정리하면 보고서를 쓰기 쉽다.

1. **실습 목적**: 어떤 함수/개념을 확인했는지
2. **실행 환경**: WSL Ubuntu, gcc 버전, Rust 버전
3. **구현 내용**: 핵심 코드와 동작 설명
4. **실행 결과**: 터미널 출력 캡처
5. **오류와 해결**: 컴파일 오류, 권한 오류, deadlock 등
6. **검토**: 배운 점, 개선할 점
