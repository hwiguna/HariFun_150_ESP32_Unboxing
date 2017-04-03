/*
  WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32 
31.01.2017 by Jan Hendrik Berlin
 
 */

#include <WiFi.h>

const char* ssid     = "Firefly24";
const char* password = "---";
int ledPin = 4;

WiFiServer server(80);

//----------------------------------
#include "esp32-hal-ledc.h"

uint32_t maxint = 65535;
uint32_t servoMax = 8000;
uint32_t servoMin = 1700;
uint32_t servoCenter = (servoMin + servoMax) / 2;
uint32_t servoLow = servoMin; //1700 + 1300;
uint32_t servoHigh = 1700 + 1000 + 1300; //servoLow+1000;

const byte numberOfServos = 12;
byte servoPins[numberOfServos] = {5, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23};

void setupServo()
{
    for (byte servoIndex = 0; servoIndex < numberOfServos; servoIndex++)
  {
    byte channel = servoIndex;
    byte servoPin = servoPins[servoIndex];

    ledcSetup(channel, 50, 16); // channel X, 50 Hz, 16-bit depth
    ledcAttachPin(servoPin, channel);   // GPIO servoPin on channel X
    ledcWrite(channel, servoHigh); //servoMin
  }
  delay(2000);
}
//----------------------------------

void setup()
{
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);      // set the LED pin mode

    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();

setupServo();
}

void Marching()
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

void LeftToRight(int servoValue, int dly)
{
  for (int head = 0; head < numberOfServos; head++)
  {
    if (head >= 0 && head < numberOfServos) ledcWrite(head, servoValue);
    delay(dly);
  }
}

void RightToLeft(int servoValue, int dly)
{
  for (int head = numberOfServos-1; head >= 0 ; head--)
  {
    if (head >= 0 && head < numberOfServos) ledcWrite(head,  servoValue);
    delay(dly);
  }
}

int value = 0;

void loop(){
 WiFiClient client = server.available();   // listen for incoming clients
int cmd=0;
  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("<h1 style='font-size:400%;'>");
            client.print("<a href='/'>Home</a><br>");
            client.print("Left to Right: <a href='/1'>&#92;&#92;&#92;</a>&nbsp;&nbsp;<a href='/2'>///</a><br>");
            client.print("Right to Left: <a href='/3'>&#92;&#92;&#92;</a>&nbsp;&nbsp;<a href='/4'>///</a><br>");
      
            client.print("All: <a href='/5'>&#92;&#92;&#92;</a>&nbsp;&nbsp;<a href='/6'>///</a><br>");
            
            client.print("<a href='/7'>Marching</a><br>");
            client.print("</h1>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /1")) cmd=1;
        if (currentLine.endsWith("GET /2")) cmd=2;
        if (currentLine.endsWith("GET /3"))cmd=3;
        if (currentLine.endsWith("GET /4"))cmd=4;
        if (currentLine.endsWith("GET /5"))cmd=5;
        if (currentLine.endsWith("GET /6"))cmd=6;
        if (currentLine.endsWith("GET /7"))cmd=7;
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
    int dly=100;
    switch (cmd)
    {
      case 1: LeftToRight(servoHigh,dly); break;
      case 2: LeftToRight(servoLow,dly); break;
      case 3: RightToLeft(servoHigh,dly); break;
      case 4: RightToLeft(servoLow,dly); break;
      case 5: LeftToRight(servoHigh,0); break;
      case 6: LeftToRight(servoLow,0); break;
      case 7: for (byte i=0; i<3; i++) Marching(); break;
    }
  }
}
