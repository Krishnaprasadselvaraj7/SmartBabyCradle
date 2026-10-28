// Soothing unit (Arduino Uno)
// A PIR sensor triggers a servo movement; when the sound sensor hears
// crying above a threshold, a DFPlayer Mini plays a lullaby (UART).

#include <Servo.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#define PIR_PIN   4
#define SERVO_PIN 5

static const uint8_t PIN_MP3_TX = 2;
static const uint8_t PIN_MP3_RX = 3;
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);

DFRobotDFPlayerMini player;
Servo myServo;

const int soundThreshold = 80;
const int soundSensorPin = A0;

void motion() {
  myServo.write(20);
  delay(500);
  myServo.write(80);
}

void music() {
  player.play(1);
  delay(15000);
}

void setup() {
  Serial.begin(9600);
  softwareSerial.begin(9600);

  if (player.begin(softwareSerial)) {
    Serial.println("OK");
    player.volume(15);  // Volume range is 0 to 30
  } else {
    Serial.println("Connecting to DFPlayer Mini failed!");
  }

  pinMode(PIR_PIN, INPUT);
  myServo.attach(SERVO_PIN);
}

void loop() {
  int pirValue = digitalRead(PIR_PIN);
  Serial.print("Motion detected = ");
  Serial.println(!pirValue);

  if (pirValue == LOW) {
    motion();
    delay(1000);
    myServo.write(50);
  }

  int soundValue = analogRead(soundSensorPin);
  Serial.print("Sound value = ");
  Serial.println(soundValue);

  if (soundValue > soundThreshold) {
    music();
  }

  delay(1000);
}
