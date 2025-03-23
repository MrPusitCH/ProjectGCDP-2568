#include <Stepper.h>

#define STEPS_PER_REV 2048  // 한 바퀴에 필요한 스텝 수 (28BYJ-48 모터 기준) / Number of steps per revolution (for 28BYJ-48 motor)

// 상태 정의 / State definitions
const int IDLE = 0;           // 대기 상태 / Idle state
const int SPEEDLEVELS = 1;    // 속도 변경 상태 / Speed change state
volatile int state = IDLE;    // 초기 상태 설정 / Initial state is IDLE

const int buttonPin = 2;      // 하나의 버튼 핀 (인터럽트용) / Button connected to Arduino pin D2

// 3단계 속도 설정 (RPM 기준) / 3 speed levels (in RPM)
const int speedLevel1 = 15;   // 약한 진동 / Low vibration
const int speedLevel2 = 30;   // 중간 진동 / Medium vibration
const int speedLevel3 = 45;   // 강한 진동 / Strong vibration

volatile int speedStage = 1;             // 현재 단계 / Current speed stage
volatile int currentSpeed = speedLevel1; // 현재 속도 / Current speed setting

// 두 개의 스텝퍼 모터 정의 / Define two stepper motors (using ULN2003 driver)
Stepper stepper1(STEPS_PER_REV, 8, 10, 9, 11); // 모터 1: IN1=D8, IN2=D10, IN3=D9, IN4=D11 / Motor 1: IN1=D8, IN2=D10, IN3=D9, IN4=D11
Stepper stepper2(STEPS_PER_REV, 4, 6, 5, 7);   // 모터 2: IN1=D4, IN2=D6, IN3=D5, IN4=D7 / Motor 2: IN1=D4, IN2=D6, IN3=D5, IN4=D7

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);  // 버튼 핀을 풀업 입력으로 설정 / Button on D2 uses INPUT_PULLUP
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButtonPress, FALLING); // D2 핀 인터럽트 설정 / Attach interrupt to pin D2

  // 초기 속도 설정 / Set initial speed
  stepper1.setSpeed(currentSpeed);
  stepper2.setSpeed(currentSpeed);
}

void loop() {
  if (state == IDLE) {
    // 모터 두 개를 한 바퀴 회전 / Rotate both motors 1 full revolution
    stepper1.step(STEPS_PER_REV);
    stepper2.step(STEPS_PER_REV);
    delay(1000);  // 1초 대기 / Wait 1 second
  }

  else if (state == SPEEDLEVELS) {
    // 속도 설정 변경 / Apply new speed setting
    stepper1.setSpeed(currentSpeed);
    stepper2.setSpeed(currentSpeed);

    // 상태를 다시 IDLE로 변경 / Return to IDLE
    state = IDLE;
  }
}

// ===========================
// 인터럽트: 버튼이 눌렸을 때 실행됨 / Interrupt: Triggered when the button is pressed
// ===========================
void handleButtonPress() {
  // 속도 단계를 순차적으로 변경 / Cycle through speed levels: 1 → 2 → 3 → 1
  speedStage++;
  if (speedStage > 3) speedStage = 1;

  // 현재 단계에 따라 속도 설정 / Set currentSpeed based on current stage
  switch (speedStage) {
    case 1:
      currentSpeed = speedLevel1;  // 약한 진동 / Low vibration
      break;
    case 2:
      currentSpeed = speedLevel2;  // 중간 진동 / Medium vibration
      break;
    case 3:
      currentSpeed = speedLevel3;  // 강한 진동 / Strong vibration
      break;
  }

  // 속도 적용 상태로 전환 / Change state to apply new speed
  state = SPEEDLEVELS;
}
