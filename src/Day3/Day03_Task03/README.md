# ATmega128 과제 및 프로젝트 템플릿

> **광운대학교 로봇학부**  
> **작성자:** 이주원
> **제출일:** 8/2

---
과제 영상링크 01: https://drive.google.com/file/d/1HWpD3OCnR2zx3OhU4XdjKzttCmGlT2-D/view?usp=sharing

과제 영상링크 02: https://drive.google.com/file/d/1WTvnaTf0CwC2cg-e1SkYkiFUsqo1aueX/view?usp=sharing
<br>- 영상 화질이 깨져서 안 보인다면 다운로드 후 보는것을 추천합니다. 
## 1. 개요 (Overview)
본 과제는 ATmega128 마이크로컨트롤러를 활용하여 주요 주변장치(스위치, UART, LCD, Dynamixel, 가변저항)를 제어하는 것을 목표로 함.

### 핵심 목표
* ATmega128를 통한 LED, 스위치, UART, LCD, Dynamixel제어
* 가변저항을 이용해서 LCD, Dynamixel제어
---

## 2. 개발 환경 (Environment)

| 항목 | 내용 |
| :--- | :--- |
| **MCU** | ATmega128A (16MHz External Crystal) |
| **IDE / Compiler** | Microchip Studio 7.0 / Microchip AVR GCC |
| **Flasher Tool** | USBISP / STK500 |
| **언어** | C Language |
| **주요 부품** | ATmega128 개발보드, LED, 스위치, LCD, 가변저항, UART, PSD, servo motor, dynamixel mx|

---

## 3. 하드웨어 구성 및 핀 맵 (Hardware Structure)

### Pin Configuration

```text
[ATmega128]                 [Target Component]
 PORTA (PA0 ~ PA7)   ----->   8-Bit LED
 PF0                  ----->   가변저항
 PD0 / PD1            ----->   I2C ISL / ISA
 PD2 / PD3 (인터럽트)   ----->  스위치 2 / 3번
 PE4 / PE5 (인터럽트)   ----->  스위치 1 / 2번
 PF1                   ----->  PSD
 PB7                   -----> SERVO motor
 PE0 / PE1 / PE2       -----> MAX485
 MAX 485               -----> Dynamixel
 ```

## 4. 프로젝트 구조 (Directory Structure)
> 구현부(.c), 선언부(.h)만 구조에 표기함.
```text
├── Day00_Task00/
    ├── Day00_Task00/
       ├── main.c
       ├── Day00_Task00.componentinfo.xml
       |── Day00_Task00.cproj
       └── 각종 헤더 파일
    ├── Day00_Task00.atsln
    └── README.md
```

---

## 5. 동작 설명 및 결과 (Results)

### 동작 시나리오
1. 가변저항 값에 따라 Dynamixel 목표 위치 설정 (0~1023)
2. PC에서 전송 받은 값(0~9)에 따라서 Dynamixel 목표 속도 설정
3. Dynamixel 목표 속도 (첫 번째 줄)와 목표 위치 (두 번째 줄) LCD에 표시

---

## 6. AI 툴 활용 명시 (AI Tools Declaration)
본 과제 작성 및 구현 과정에서 활용한 AI 도구(Generative AI)의 사용 현황 및 목적은 다음과 같음.

| 도구명 (Tool) | 활용 영역 | 세부 사용 목적 및 내용 |
| :--- | :--- | :--- |
| **Claude** | 코드 디버깅 / UART 학습 / Dynamixel 제어 / 프로토콜 2.0 학습 | - 빌드 에러 및 문법 오류 원인 분석<br>- 레지스터, 마이크로컨트롤러, 다이나믹셀에 관한 사용법 및 예제코드 분석, 2.0 프로토콜에 대한 사용법 일부 학습 |

### AI 활용 및 검증 원칙
1. **학습 주도성:** 코드의 핵심 제어 로직 설계는 직접 작성하였으며, AI는 보조 도구 로만 활용하였습니다.
