#include <ESP32Servo.h>
const byte PIN_SERVO = 13;
const int MIN_US = 500; 
const int MAX_US = 2400;
Servo sg90;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Test SG90 - 0…180°");

  sg90.setPeriodHertz(50);  // 50 Hz PWM pentru servo
  sg90.attach(PIN_SERVO, MIN_US, MAX_US);
}

void loop() {
  // de la 0 la 180 grade 
  for (int ang = 0; ang <= 180; ang += 10) {
    sg90.write(ang);
    Serial.printf("Unghi = %3d°\n", ang);
    delay(300);                  // ~300 ms între poziții
  }

  // de la 180 la 0 grade
  for (int ang = 180; ang >= 0; ang -= 10) {
    sg90.write(ang);
    Serial.printf("Unghi = %3d°\n", ang);
    delay(300);
  }
}
