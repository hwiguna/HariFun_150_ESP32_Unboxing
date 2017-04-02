#include "esp32-hal-ledc.h"

uint32_t maxint = 65535;
uint32_t servoMax = 8000;
uint32_t servoMin = 1700;

void setup() {
  Serial.begin(9600);

  ledcSetup(1, 50, 16); // channel 1, 50 Hz, 16-bit depth
  ledcAttachPin(22, 1);   // GPIO 22 on channel 1

  ledcSetup(2, 50, 16); // channel 1, 50 Hz, 16-bit depth
  ledcAttachPin(23, 2);   // GPIO 22 on channel 1
}

void loop() {
  int potValue = analogRead(A0);
  Serial.print(potValue);
  uint32_t servoPos = map(potValue, 0, 4095, servoMin, servoMax);
  Serial.print(" -> ");
  Serial.println(servoPos);
  ledcWrite(1, servoPos);
  
  ledcWrite(2, servoPos);
  delay(500);
}
