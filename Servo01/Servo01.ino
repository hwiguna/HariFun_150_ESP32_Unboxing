#include "esp32-hal-ledc.h"

uint32_t maxint = 65535;
uint32_t servoMax = 8000;
uint32_t servoMin = 1700;
uint32_t servoCenter = (servoMin+servoMax)/2;
uint32_t servoLow = 1700+1300;

void setup() {
  Serial.begin(9600);

  ledcSetup(1, 50, 16); // channel 1, 50 Hz, 16-bit depth
  ledcAttachPin(22, 1);   // GPIO 22 on channel 1

  ledcSetup(2, 50, 16); // channel 1, 50 Hz, 16-bit depth
  ledcAttachPin(23, 2);   // GPIO 22 on channel 1
}

void loop() {
  ledcWrite(1, servoLow);
  ledcWrite(2, servoLow);
  delay(500);
  ledcWrite(1, servoCenter);
  ledcWrite(2, servoCenter);
  delay(500);
}
