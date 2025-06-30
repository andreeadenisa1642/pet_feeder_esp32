#include "HX711.h"
#define DT 4
#define SCK 23

HX711 scale;
//float calib= -224.3;

void setup(){
  Serial.begin(115200);
  scale.begin(DT, SCK);
  scale.set_scale(-234.7);
  delay(2000);
  scale.tare();
  Serial.println("Calibrat.");
}

void loop(){
  float greutate=scale.get_units(20); // media a 20 citiri
  if (abs(greutate)<2) greutate=0; //eliminam zgomot sub 2g
  Serial.print("Greutate: ");
  Serial.print(greutate, 2);
  //Serial.println(scale.read());
  Serial.println(" g");
  delay(500);
}

/*#include "HX711.h"

HX711 scale;

void setup() {
  Serial.begin(115200);
  scale.begin(4, 23);
  if (!scale.is_ready()) {
    Serial.println("HX711 NU e pregătit.");
    while (1);
  }
}

void loop() {
  Serial.print("Valoare brută: ");
  Serial.println(scale.read());
  delay(500);
}
*/
