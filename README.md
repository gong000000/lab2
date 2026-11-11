# Lab2

파일 처리, 프로세스, 프로세스 간 통신 실습 과제입니다.

## 구성

- 2번: `rust/matrix_add` (Rust)
- 3~10번: `c/` C 프로그램
- 11번: `c/11_dbus_server.c`, `c/11_dbus_client.c`
- 12번 팀 과제: 추후 진행

## 빌드

### 2번 (Rust)

```bash
cd rust/matrix_add
cargo build
```

### 3~11번 (C)

```bash
cd c
make
```

`c/bin/`에 실행 파일이 생성됩니다.

11번 DBus는 개발 패키지가 필요합니다.

```bash
sudo apt install -y libdbus-1-dev pkg-config dbus
```

## 실행

### 2번

```bash
cd rust/matrix_add
cargo run
```

행의 개수와 열의 개수를 입력한 뒤, 각 행의 값을 공백으로 구분해 입력합니다.

### 3~10번

```bash
cd c

./bin/03_file_dir
./bin/04_ls_r .
./bin/05_typing
./bin/06_process_signal
./bin/07_ipc_practice
./bin/08_pipe_chat alice
./bin/09_mq_chat --clean
./bin/10_mmap_copy source.txt copy.txt
```

8번, 9번은 터미널 2개에서 각각 `alice`, `bob`으로 실행합니다.
10번은 원본 파일이 먼저 있어야 합니다.

### 11번

```bash
cd c
dbus-run-session -- bash

# 같은 셸 안에서
./bin/11_dbus_server &
./bin/11_dbus_client "Hello"
```

서버 쪽에 `[알림] Hello`가 출력됩니다.
실행 예시는 `c/README.md`에도 있습니다.
