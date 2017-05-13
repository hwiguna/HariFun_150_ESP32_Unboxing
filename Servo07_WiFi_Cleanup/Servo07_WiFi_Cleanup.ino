#include <WiFi.h>

const char* ssid     = "Firefly24";
const char* password = "Buckbeak&Stu";
int ledPin = 4;

WiFiServer server(80);

//----------------------------------
#include "esp32-hal-ledc.h"

uint32_t maxint = 65535;
uint32_t servoMax = 8000;
uint32_t servoMin = 1700;
uint32_t servoLow = servoMin; //1700 + 1300;
uint32_t servoHigh = 1700 + 1000 + 1300; //servoLow+1000;
uint32_t servoCenter = (servoLow + servoHigh) / 2;

const byte numberOfServos = 12;
byte servoPins[numberOfServos] = {5, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23};
char command = 0;

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
  for (int head = numberOfServos - 1; head >= 0 ; head--)
  {
    if (head >= 0 && head < numberOfServos) ledcWrite(head,  servoValue);
    delay(dly);
  }
}

void SpreadOut(int dly)
{
  int half = (numberOfServos / 2);
  for (int i = 0; i <= half ; i++)
  {
    int left = half - i;
    int right = half + i;
    if (left >= 0 && left < numberOfServos) ledcWrite(left,  servoHigh);
    if (right >= 0 && right < numberOfServos) ledcWrite(right,  servoLow);
    delay(dly);
  }
}
void SpreadOutRestore(int dly)
{
  int half = (numberOfServos / 2);
  for (int i = 0; i <= half ; i++)
  {
    int left = half - i;
    int right = half + i;
    if (left >= 0 && left < numberOfServos) ledcWrite(left,  servoCenter);
    if (right >= 0 && right < numberOfServos) ledcWrite(right,  servoCenter);
    delay(dly);
  }
}
void OutIn(int dly)
{
  int half = (numberOfServos / 2);
  for (int i = 0; i <= half ; i++)
  {
    int left = (numberOfServos - 1) - i;
    int right = i;
    if (left >= 0 && left < numberOfServos) ledcWrite(left,  servoCenter);
    if (right >= 0 && right < numberOfServos) ledcWrite(right,  servoCenter);
    delay(dly);
  }
}

int value = 0;

void SendHtmlResponse(WiFiClient client)
{
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();

  // the content of the HTTP response follows the header:
  client.print("<h1 style='font-size:400%;'>");
  client.print("<a href='/'>Home</a><br>");
  client.print("Left to Right: <a href='/A'>&#92;&#92;&#92;</a>&nbsp;&nbsp;<a href='/B'>///</a><br>");
  client.print("Right to Left: <a href='/C'>&#92;&#92;&#92;</a>&nbsp;&nbsp;<a href='/D'>///</a><br>");
  client.print("All: <a href='/E'>&#92;&#92;&#92;</a>&nbsp;&nbsp;<a href='/H'>Center</a>&nbsp;&nbsp;<a href='/F'>///</a><br>");
  client.print("<a href='/G'>Marching</a><br>");
  client.print("<a href='/I'>SpreadOut</a>&nbsp;&nbsp;<a href='/J'>SpreadOutRestore</a><br>");
  client.print("<a href='/K'>Bounce</a><br>");
  client.print("</h1>");

  // The HTTP response ends with another blank line:
  client.println();
}

void ProcessRequest(String requestString)
{
  // GET /x HTTP1.1
  // 0123456789
  if (requestString.length() >= 6 && requestString.startsWith("GET /") && !requestString.startsWith("GET /favicon.ico"))
  {
    command = requestString[5];
  }
}

void loop() {
  WiFiClient client = server.available();   // listen for incoming clients
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
            SendHtmlResponse(client);
            break; // break out of the while loop:
          } else {    // if you got a newline, then clear currentLine:
            ProcessRequest(currentLine);
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
    
    int dly = 100;
    switch (command)
    {
      case 'A': LeftToRight(servoHigh, dly); break;
      case 'B': LeftToRight(servoLow, dly); break;
      case 'C': RightToLeft(servoHigh, dly); break;
      case 'D': RightToLeft(servoLow, dly); break;
      case 'E': LeftToRight(servoHigh, 0); break;
      case 'F': LeftToRight(servoLow, 0); break;
      case 'G': for (byte i = 0; i < 3; i++) Marching(); break;
      case 'H': LeftToRight(servoCenter, 0); break;
      case 'I': SpreadOut(dly); break;
      case 'J': SpreadOutRestore(dly); break;
      case 'K':
        LeftToRight(servoLow, dly);
        RightToLeft(servoHigh, dly);
        LeftToRight(servoCenter, dly);
        break;
    }
    command = 0;
  }
}
