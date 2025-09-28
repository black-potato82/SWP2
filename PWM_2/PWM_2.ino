#define LED_PIN       7        // LED 연결 핀 번호
#define MIN_DUTY      0        // 최소 듀티비 (%)
#define MAX_DUTY      100      // 최대 듀티비 (%)
#define MIN_PERIOD    100      // 최소 주기 (us)
#define MAX_PERIOD    10000    // 최대 주기 (us)
#define DUTY_STEP     1        // 밝기 변화 단위 (%)
#define PWM_DURATION  5        // 각 단계에서 유지할 시간 (ms)

unsigned int pwm_period = 1000;  // 기본 PWM 주기 (us)
unsigned int pwm_duty = 50;      // 기본 듀티 (%)


void set_period(unsigned int period) {
  if (period >= MIN_PERIOD && period <= MAX_PERIOD) {
    pwm_period = period;
  }
}


void set_duty(unsigned int duty) {
  if (duty >= MIN_DUTY && duty <= MAX_DUTY) {
    pwm_duty = duty;
  }
}

// PWM 출력
void softwarePWM() {
  unsigned int high_time = pwm_period * pwm_duty / 100;
  unsigned int low_time = pwm_period - high_time;

  digitalWrite(LED_PIN, HIGH);
  delayMicroseconds(high_time);

  digitalWrite(LED_PIN, LOW);
  delayMicroseconds(low_time);
}

// LED 밝기
void PWMbrightness() {
  unsigned int duty = MIN_DUTY;
  bool increasing = true;

  // 200단계 = 1초 (200 * 5ms = 1000ms)
  for (int i = 0; i < 200; i++) {
    set_duty(duty);

    // 각 단계에서 PWM_DURATION(ms) 동안 PWM 실행
    unsigned long start_time = millis();
    while (millis() - start_time < PWM_DURATION) {
      softwarePWM();
    }

    // 듀티 값 업데이트
    if (increasing) {
      duty += DUTY_STEP;
      if (duty >= MAX_DUTY) {
        duty = MAX_DUTY;
        increasing = false;
      }
    } else {
      duty -= DUTY_STEP;
      if (duty <= MIN_DUTY) {
        duty = MIN_DUTY;
        increasing = true;
      }
    }
  }
}



void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  set_period(10000);  // 10ms
  PWMbrightness();
  delay(1000);

  set_period(1000);   // 1ms
  PWMbrightness();
  delay(1000);

  set_period(100);    // 0.1ms
  PWMbrightness();
  delay(1000);
}
