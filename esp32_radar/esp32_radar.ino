#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include "html.h"

const char* ssid = "Dev";
const char* password = "12345678";

#define LED 2
#define TrigPin 5
#define EchoPin 18
#define SoundSpeed 0.034

WebServer server(80);
Servo servo;

float distance = 0;
unsigned long previousMillis = 0;
unsigned long currentMillis;
int period = 80;
int servoAngle = 90;
bool servoDirection = true;


void MainPage() {
  String _html_page = html_page;
  server.send(200, "text/html", _html_page);
}

void Distance() {
  String data = "[\""+String(distance)+"\",\""+String(servoAngle)+"\"]";
  server.send(200, "text/plane", data);
}

void DistanceCalculator()
{
  digitalWrite(TrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(TrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);

  long duration = pulseIn(EchoPin, HIGH);
  distance = duration * SoundSpeed / 2;

}

void servoController() {
  currentMillis = millis();
  if (servoAngle > 165)
    servoDirection = false;
  else if (servoAngle <= 15)
    servoDirection = true;

  if (currentMillis - previousMillis >= period) {

    if (servoDirection)
      servo.write(++servoAngle);

    else if (!servoDirection)
      servo.write(--servoAngle);

    DistanceCalculator();

    Serial.print("Distance : ");
    Serial.print(distance);
    Serial.print("\t\tServo Angle: ");
    Serial.println(servoAngle);

    previousMillis = currentMillis;
  }
}

void setup()
{
  servo.attach(13);
  servo.write(servoAngle);
  pinMode(TrigPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  WiFi.mode(WIFI_STA);    
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);
    delay(100);
    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);
    Serial.print(".");
    delay(500);
  }
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  server.on("/", MainPage);
  server.on("/readDistance", Distance);

  server.begin();
}

void loop()
{
  server.handleClient();
  servoController();
}
