#include "esp32-hal-ledc.h"

uint32_t maxint = 65535;
uint32_t servoMax = 8000;
uint32_t servoMin = 1700;
uint32_t servoCenter = (servoMin + servoMax) / 2;
uint32_t servoLow = servoMin; //1700 + 1300;
uint32_t servoHigh = 1700 + 1000; //servoLow+1000;

const byte numberOfServos = 12;
byte servoPins[numberOfServos] = {5,12,13,14,15,16,17,18,19,21,22,23};

void setup() {
  //Serial.begin(9600);

  for (byte servoIndex = 0; servoIndex < numberOfServos; servoIndex++)
  {
    byte channel = servoIndex;
    byte servoPin = servoPins[servoIndex];
    
    ledcSetup(channel, 50, 16); // channel X, 50 Hz, 16-bit depth
    ledcAttachPin(servoPin, channel);   // GPIO servoPin on channel X
    ledcWrite(channel, servoMin);
  }
  delay(2000);
}

void loop() {
  for (byte servoIndex=0; servoIndex < numberOfServos; servoIndex++)
  {
    byte channel = servoIndex;
    ledcWrite(channel, servoHigh);
    delay(500);
    ledcWrite(channel, servoLow);
    delay(500);
  }
}
