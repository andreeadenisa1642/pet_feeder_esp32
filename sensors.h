#pragma once
#include <Arduino.h>
#include <HX711.h>
#include "config.h"

extern HX711 scale;

void initUltrasonic() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void initScale() {
  scale.begin(DT_PIN, SCK_PIN);
  scale.set_scale(-234.0f);   // factor de calibrare
  scale.tare();
}

float readDistance() {
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(4);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30 ms timeout
  return duration * 0.0343f / 2.0f; // in cm
}

inline float readWeight() {
  const uint8_t samples = 10;
  float sum = 0;
  for(uint8_t i=0; i<samples; i++) sum += scale.get_units();
  return sum / samples; //grame
}
