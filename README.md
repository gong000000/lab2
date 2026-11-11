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
├── rust/
│   └── matrix_add/          # 2번: Rust Vec<T> 행렬 덧셈
│       ├── Cargo.toml
│       └── src/main.rs
└── c/
    ├── Makefile             # 3~12번 C 프로그램 공통 빌드
    └── README.md            # 과제별 파일/핵심 함수 정리
```

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

### 3~12번 — C 프로그램

`c/` 디렉토리에 `03_file_dir.c`, `04_ls_r.c` 같은 파일을 만들면
Makefile이 자동으로 `c/bin/` 아래에 실행 파일을 만듭니다.

```bash
cd /mnt/c/Users/gong0000/Desktop/lab2/c
make                    # c/*.c 전체 빌드
make bin/04_ls_r        # 하나만 빌드
./bin/03_file_dir       # 3번: 파일/디렉토리 함수 연습
./bin/04_ls_r .         # 4번: 현재 디렉토리를 ls -R처럼 출력
```

## 실습 순서 체크리스트

- [x] **1.** GitHub 저장소에 lab2 프로젝트 생성 및 업로드
- [x] **2.** Rust `Vec<T>`를 사용한 두 행렬의 덧셈
- [x] **3.** 파일/디렉토리 관련 함수 연습
- [x] **4.** 디렉토리 재귀 탐색 (`ls -R`와 동일한 결과)
- [ ] **5.** 타자 연습 프로그램 (오타 횟수, 분당 타자수)
- [ ] **6.** 프로세스/시그널 관련 함수 연습
- [ ] **7.** 프로세스 간 통신(IPC) 함수 연습
- [ ] **8.** 파이프 기반 텍스트 채팅
- [ ] **9.** POSIX 메시지 큐 기반 텍스트 채팅
- [ ] **10.** POSIX 공유 메모리/mmap 파일 복사
- [ ] **11.** DBus 알림 메시지 서버-클라이언트
- [ ] **12.** (팀) 간단한 쉘 프로그램

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
