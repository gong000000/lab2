# Lab2 — 파일 처리, 프로세스 및 프로세스간 통신

이 저장소는 `Lab2.pdf`의 **5. 실습 순서**에 있는 과제를 진행하기 위한 프로젝트입니다.

## 실행 환경

- WSL2 Ubuntu (Windows의 `C:\Users\gong0000\Desktop\lab2`를 `/mnt/c/Users/gong0000/Desktop/lab2`로 접근)
- Rust (`rustup`, `cargo`)
- gcc, make, git

> Windows PowerShell이 아니라 **WSL Ubuntu 터미널**에서 빌드/실행하세요.
> Rust는 WSL 쪽에 설치되어 있습니다.

## 디렉토리 구조

```text
lab2/
├── README.md
├── .gitignore
├── docs/
│   └── step-by-step.md      # 5~12번 상세 가이드
├── rust/
│   └── matrix_add/          # 2번: Rust Vec<T> 행렬 덧셈
│       ├── Cargo.toml
│       └── src/main.rs
└── c/
    ├── Makefile             # 3~11번 C 프로그램 공통 빌드
    ├── 03_file_dir.c        # 3번: 파일/디렉토리 함수 연습
    ├── 04_ls_r.c            # 4번: ls -R 구현
    ├── 05_typing.c          # 5번: 타자 연습
    ├── 06_process_signal.c  # 6번: 프로세스/시그널
    ├── 07_ipc_practice.c    # 7번: IPC 연습
    ├── 08_pipe_chat.c       # 8번: 파이프 채팅
    ├── 09_mq_chat.c         # 9번: 메시지 큐 채팅
    ├── 10_mmap_copy.c       # 10번: mmap 파일 복사
    ├── 11_dbus_server.c     # 11번: DBus 서버
    ├── 11_dbus_client.c     # 11번: DBus 클라이언트
    └── README.md            # 과제별 파일/핵심 함수 정리
```

5~11번의 자세한 구현 순서와 핵심 함수는
[`docs/step-by-step.md`](docs/step-by-step.md)를 참고하세요.

## 빌드와 실행

### 2번 — Rust 행렬 덧셈

```bash
cd /mnt/c/Users/gong0000/Desktop/lab2/rust/matrix_add
cargo run
```

실행 예:

```text
행의 개수: 2
열의 개수: 2
행렬 A의 1/2행 (2개 값): 1 2
행렬 A의 2/2행 (2개 값): 3 4
행렬 B의 1/2행 (2개 값): 5 6
행렬 B의 2/2행 (2개 값): 7 8

A =
     1      2
     3      4

B =
     5      6
     7      8

A + B =
     6      8
    10     12
```

### 3~11번 — C 프로그램

```bash
cd /mnt/c/Users/gong0000/Desktop/lab2/c
make                    # 3~10번 + (DBus 패키지가 있으면) 11번 빌드
make bin/09_mq_chat     # 하나만 빌드

./bin/03_file_dir       # 3번: 파일/디렉토리 함수 연습
./bin/04_ls_r .         # 4번: 현재 디렉토리를 ls -R처럼 출력
./bin/05_typing         # 5번: 타자 연습
./bin/06_process_signal # 6번: 프로세스/시그널
./bin/07_ipc_practice   # 7번: IPC 연습
./bin/08_pipe_chat alice
./bin/09_mq_chat --clean
echo "hello mmap" > source.txt
./bin/10_mmap_copy source.txt copy.txt
```

11번 DBus는 개발 패키지가 필요합니다.

```bash
sudo apt install -y libdbus-1-dev pkg-config dbus
make bin/11_dbus_server bin/11_dbus_client
dbus-run-session -- bash
# 같은 셸에서 서버와 클라이언트 실행
./bin/11_dbus_server &
./bin/11_dbus_client "Hello"
```

## 실습 순서 체크리스트

- [x] **1.** GitHub 저장소에 lab2 프로젝트 생성 및 업로드
- [x] **2.** Rust `Vec<T>`를 사용한 두 행렬의 덧셈
- [x] **3.** 파일/디렉토리 관련 함수 연습
- [x] **4.** 디렉토리 재귀 탐색 (`ls -R`와 동일한 결과)
- [x] **5.** 타자 연습 프로그램 (오타 횟수, 분당 타자수)
- [x] **6.** 프로세스/시그널 관련 함수 연습
- [x] **7.** 프로세스 간 통신(IPC) 함수 연습
- [x] **8.** 파이프 기반 텍스트 채팅
- [x] **9.** POSIX 메시지 큐 기반 텍스트 채팅
- [x] **10.** POSIX 공유 메모리/mmap 파일 복사
- [x] **11.** DBus 알림 메시지 서버-클라이언트
- [ ] **12.** (팀) 간단한 쉘 프로그램 — 이번 범위에서 제외

## Git 작업 순서

빌드/실행은 WSL에서 하고, **Git 명령은 Windows PowerShell**에서 실행하는 것을 권장합니다.
(WSL의 git에는 GitHub 로그인 정보가 없어서 push할 때 인증을 다시 요구할 수 있습니다.)

```powershell
cd C:\Users\gong0000\Desktop\lab2

git status
git add .
git commit -m "feat: add typing practice"
git push
```

과제를 하나 끝낼 때마다 위처럼 `add → commit → push`를 반복하면 됩니다.

## 보고서에 기록할 내용

각 과제마다 다음을 캡처하거나 정리해 두세요.

1. 실행 환경과 컴파일 명령
2. 소스 코드의 핵심 부분
3. 실행 결과 화면
4. 발생한 오류와 해결 과정
5. 배운 점 / 검토 사항
