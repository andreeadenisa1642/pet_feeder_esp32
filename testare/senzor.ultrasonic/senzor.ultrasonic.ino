const byte PIN_TRIG = 5; //OUT
const byte PIN_ECHO = 18; //IN

void setup() {
  Serial.begin(115200);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
  delay(2000);  
  Serial.println("Test senzor ultrasonic - distanţa în cm");
}

void loop() {
  digitalWrite(PIN_TRIG, HIGH); 
  delayMicroseconds(10); //impuls TRIG 10 µs
  digitalWrite(PIN_TRIG, LOW);

  long durata = pulseIn(PIN_ECHO, HIGH, 30000);   // timeout 30 ms
  if (durata == 0) {
    Serial.println("Fără ecou (prea departe sau timeout)");
  } else {
    float distanta = durata / 58.0;
    Serial.print("Distanta = ");
    Serial.print(distanta, 1);
    Serial.println(" cm");
  }
  delay(500);
}
