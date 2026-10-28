// Cradle receiver (ESP32)
// Receives wrist-band accelerometer data over nRF24L01 (SPI), maps the
// X-axis tilt to a servo angle to rock the cradle, and checks a raindrop
// (wet-bed) sensor on a non-blocking 2.5 s timer, sending a Blynk alert.

#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "BabyCradle"
#define BLYNK_AUTH_TOKEN    "YOUR_BLYNK_AUTH_TOKEN"
#define BLYNK_PRINT Serial

#include <Wire.h>
#include <RF24.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

#define RAIN_SENSOR 16

BlynkTimer timer;
int flag = 0;
RF24 radio(22, 21);  // CE, CSN pins on ESP32
Servo motor;

// Called every 2.5 s by the timer: alert once when the bed becomes wet
void checkWetBed() {
  int sensorState = digitalRead(RAIN_SENSOR);

  if (sensorState == 0 && flag == 0) {
    Serial.println("Wet bed detected");
    Blynk.logEvent("notifications", "Wet bed detected");
    flag = 1;
  } else if (sensorState == 1) {
    flag = 0;
  }
}

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.openReadingPipe(1, 0xF0F0F0F0E1LL);  // Same address as the transmitter
  radio.startListening();

  motor.attach(4);  // Rocking servo on GPIO 4
  pinMode(RAIN_SENSOR, INPUT);

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(2500L, checkWetBed);
}

void loop() {
  if (radio.available()) {
    char dataPacket[32];
    radio.read(&dataPacket, sizeof(dataPacket));

    // Parse the accelerometer packet
    int16_t ax, ay, az;
    sscanf(dataPacket, "%d,%d,%d", &ax, &ay, &az);

    // Map wrist tilt to a servo angle
    int servoAngle = map(ax, -16000, 16000, 0, 180);
    motor.write(constrain(servoAngle, 0, 180));

    Serial.print("Ax: ");  Serial.print(ax);
    Serial.print(", Ay: "); Serial.print(ay);
    Serial.print(", Az: "); Serial.print(az);
    Serial.print(", Servo: "); Serial.println(servoAngle);
  }

  Blynk.run();
  timer.run();
}
