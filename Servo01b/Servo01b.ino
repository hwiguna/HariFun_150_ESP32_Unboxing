#include "esp32-hal-ledc.h"

int frequency = 50; // in Hertz
int resolution = 16; // 1..16 bits
int servoMax = 8000;
int servoMin = 1700;

int servoChannel = 0; // 0 through 15
int servoPin = 23; // Any GPIO pin!

int potpin = 36;  // analog pin used to connect the potentiometer

void setup() {
  ledcSetup(servoChannel, frequency, resolution); // channel 1, 50 Hz, 16-bit depth
  ledcAttachPin(servoPin, servoChannel);   // GPIO 22 on channel 1
}

void loop() {
  int potValue = analogRead(potpin); // reads the value of the potentiometer (value between 0 and 1023)
  int servoValue = map(potValue, 0, 4095, servoMin, servoMax);
  ledcWrite(servoChannel, servoValue);
  delay(15);               // waits for the servo to get there
}
