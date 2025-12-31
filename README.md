# 📘 프로젝트 개요 (Project Overview)

본 저장소는 RGT 과제 수행을 위해 구현한 C++ 시스템 프로그래밍 과제와 RESTful API 서버 구현 결과물을 정리한 저장소입니다.
자료구조, 멀티스레딩, 파일 관리, 그리고 REST 기반 서버 설계를 통해 소프트웨어 구조 설계 및 구현 역량을 종합적으로 검증하는 것을 목표로 하였습니다.

각 과제는 기능 단위 디렉터리로 분리하여 구성하였으며, Git 브랜치를 활용해 단계적으로 개발·병합하였습니다.

---

## 🛠️ 설치 및 실행 환경 (Installation Environment)

### ✅ 공통 개발 환경

| 항목 | 내용 |
|------|------|
| OS | Windows 10 / 11 |
| 언어 | C++17 / Python 3.9 |
| IDE | Visual Studio Code |
| 컴파일러 | g++ (MSYS2 UCRT64) |
| 형상관리 | Git / GitHub |

---

## ✅ 구현 과제 상세 설명

### Log File Manager (로그 파일 관리)

#### 목적
파일 입출력과 RAII 개념을 활용하여 안정적인 로그 관리 클래스를 구현하였습니다.

#### 주요 기능
- `std::unique_ptr` 기반 리소스 관리
- 복사 생성 / 대입 삭제
- 이동 생성 / 대입 허용
- 시간 기반 로그 기록
- 예외 처리

#### 실행방법
```bash
./LogFileManager.exe
```

---

### Circular Buffer (원형 버퍼)

#### 목적
고정 크기 버퍼를 원형 구조로 구현하여 메모리를 효율적으로 관리하고, 큐 형태의 자료구조를 직접 구현하였습니다.

#### 주요 기능
- push / pop
- overflow 방지
- iterator 구현
- range-based for 지원

#### 실행 방법
```bash
./CircularBuffer.exe
```

---

### Parallel Processor (멀티스레딩 병렬 처리)

#### 목적
`std::async`를 활용해 다수의 작업을 병렬로 처리하는 구조를 구현하여, 멀티스레딩 기반 처리 구조 이해를 목표로 하였습니다.

#### 주요 기능
- 템플릿 기반 `parallel_map`
- `std::future`를 이용한 비동기 실행
- 입력 순서 유지
- 스레드 안전한 결과 관리

#### 실행 방법
```bash
./main.exe
```

---

## RESTful API 서버 구현

### RESTful API 개발 환경

| 항목 | 내용 |
|------|------|
| Python | 3.9 |
| Framework | FastAPI |
| Server | Uvicorn |
| API Style | REST |
| 데이터 포맷 | JSON |
| 테스트 | Swagger UI |
| 가상환경 | venv |

### REST API 구성
- FastAPI 기반 서버
- JSON 기반 통신

### 실행 방법
```bash
cd restful-api
python -m venv venv
venv\Scripts\activate
pip install -r requirements.txt
uvicorn app.main:app --reload
```