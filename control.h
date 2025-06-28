#pragma once
#include <Arduino.h>
#include <ESP32Servo.h>
#include "config.h"

extern Servo dispenserServo;

inline void initServo() {
  dispenserServo.attach(SERVO_PIN);
  dispenserServo.write(0);  // pozitia initala la 0 grade
}

void serveFood() {
  dispenserServo.write(110);  // deschis la 110 grade
  delay(2000); // deschis pentru 2 secunde
  dispenserServo.write(0);    // inchis
}