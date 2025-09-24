// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12   // sonar sensor TRIGGER
#define PIN_ECHO 13   // sonar sensor ECHO

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25      // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100.0   // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300.0   // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL) // coefficent to convert duration to distance

unsigned long last_sampling_time;   // unit: msec

void setup() {
  // initialize GPIO pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);  // sonar TRIGGER
  pinMode(PIN_ECHO, INPUT);   // sonar ECHO
  digitalWrite(PIN_TRIG, LOW);  // turn-off Sonar 
  
  // initialize serial port
  Serial.begin(57600);
}

void loop() { 
  float distance;

  if (millis() < (last_sampling_time + INTERVAL))
    return;

  distance = USS_measure(PIN_TRIG, PIN_ECHO);

  // 밝기 조절용 변수
  int pwm_value = 255; // 기본적으로 LED 꺼짐

  if ((distance == 0.0) || (distance > _DIST_MAX)) {
      distance = _DIST_MAX + 10.0;
      pwm_value = 255; // LED OFF
  } else if (distance < _DIST_MIN) {
      distance = _DIST_MIN - 10.0;
      pwm_value = 255; // LED OFF
  } else {
      // 100mm ~ 300mm 사이일 때만 밝기 비례 계산
      // 최대 밝기: 200mm일 때
      // 최소 밝기: 100mm, 300mm일 때

      float diff = abs(distance - 200.0); // 기준 거리 200mm에서 얼마나 떨어졌는가
      float brightness_ratio = diff / 100.0; // 0.0 (200mm) ~ 1.0 (100mm or 300mm)
      brightness_ratio = constrain(brightness_ratio, 0.0, 1.0); // 예외 방지

      pwm_value = int(255.0 * brightness_ratio); // 0~255 사이로 맵핑
  }

  analogWrite(PIN_LED, pwm_value);

  // 디버깅 출력
  Serial.print("Distance: "); Serial.print(distance);
  Serial.print(" mm, PWM: "); Serial.println(pwm_value);

  last_sampling_time += INTERVAL;
}


// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm

  // Pulse duration to distance conversion example (target distance = 17.3m)
  // - pulseIn(ECHO, HIGH, timeout) returns microseconds (음파의 왕복 시간)
  // - 편도 거리 = (pulseIn() / 1,000,000) * SND_VEL / 2 (미터 단위)
  //   mm 단위로 하려면 * 1,000이 필요 ==>  SCALE = 0.001 * 0.5 * SND_VEL
  //
  // - 예, pusseIn()이 100,000 이면 (= 0.1초, 왕복 거리 34.6m)
  //        = 100,000 micro*sec * 0.001 milli/micro * 0.5 * 346 meter/sec
  //        = 100,000 * 0.001 * 0.5 * 346
  //        = 17,300 mm  ==> 17.3m
}
