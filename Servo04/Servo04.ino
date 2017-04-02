#include "esp32-hal-ledc.h"

uint32_t maxint = 65535;
uint32_t servoMax = 8000;
uint32_t servoMin = 1700;
uint32_t servoCenter = (servoMin + servoMax) / 2;
uint32_t servoLow = servoMin; //1700 + 1300;
uint32_t servoHigh = 1700 + 1000 + 1300; //servoLow+1000;

const byte numberOfServos = 12;
byte servoPins[numberOfServos] = {5, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23};

void setup() {
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

void InStep()
{
  for (byte servoIndex = 0; servoIndex < numberOfServos; servoIndex++)
  {
    byte channel = servoIndex;
    ledcWrite(channel, servoHigh);
  }
  delay(500);
  for (byte servoIndex = 0; servoIndex < numberOfServos; servoIndex++)
  {
    byte channel = servoIndex;
    ledcWrite(channel, servoLow);
  }
  delay(500);
}

//int Validate(int x)
//{
//  int r = x;
//  if (x < 0) r = numberOfServos - 1 - x;
//  if (x > numberOfServos) r =  x - numberOfServos - 1;
//  return r;
//}

void TheWave1()
{
  byte dly = 100;

  for (byte servoIndex = 0; servoIndex < numberOfServos; servoIndex++)
  {
    byte channel = servoIndex;
    ledcWrite(channel,  servoHigh);
    delay(dly);
    ledcWrite(channel, servoLow);
    delay(dly);
  }

  for (byte servoIndex = numberOfServos - 1; servoIndex > 0; servoIndex--)
  {
    byte channel = servoIndex;
    ledcWrite(channel, servoHigh);
    delay(dly);
    ledcWrite(channel, servoLow);
    delay(dly);
  }
}

void TheWave2()
{
  byte dly = 100;
  int trail = 1;
  for (int head = 0; head < (numberOfServos + trail - 1); head++)
  {
    if (head >= 0 && head < numberOfServos) ledcWrite(head,  servoLow);
    int tail = head - trail;
    if (tail >= 0 && tail < numberOfServos) ledcWrite(tail,  servoHigh);
    delay(dly);
  }

  for (int head = (numberOfServos-1); head >= -trail ; head--)
  {
    if (head >= 0 && head < numberOfServos) ledcWrite(head,  servoLow);
    int tail = head + trail;
    if (tail >= 0 && tail < numberOfServos) ledcWrite(tail,  servoHigh);
    delay(dly);
  }
}

void loop() {
  TheWave2();
  delay(500);
}
