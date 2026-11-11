# Lab2

파일 처리, 프로세스, 프로세스 간 통신 실습 과제입니다.

## 구성

- `rust/matrix_add`: 2번 행렬 덧셈 (Rust)
- `c/`: 3~11번 C 프로그램
- 12번 팀 과제는 추후 진행

## 빌드

### Rust

```bash
cd rust/matrix_add
cargo run
```

### C

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
11번은 `dbus-run-session -- bash` 안에서 서버와 클라이언트를 실행합니다.
실행 예시는 `c/README.md`에 있습니다.
