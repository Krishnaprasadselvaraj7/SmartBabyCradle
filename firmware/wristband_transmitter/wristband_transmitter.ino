// Wrist band transmitter (ESP32)
// Reads the MPU6050 accelerometer over I2C and sends the readings
// to the cradle receiver over an nRF24L01 2.4 GHz radio (SPI).

#include <Wire.h>
#include <MPU6050.h>
#include <RF24.h>
#include <RF24_config.h>

MPU6050 mpu;
RF24 radio(27, 26);  // CE, CSN pins on ESP32

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();

  radio.begin();
  radio.openWritingPipe(0xF0F0F0F0E1LL);  // Address shared with the receiver
}

void loop() {
  // Read accelerometer data from the MPU6050
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Pack the three axes into a text packet
  char dataPacket[32];
  snprintf(dataPacket, sizeof(dataPacket), "%d,%d,%d", ax, ay, az);

  // Send the packet wirelessly
  radio.write(&dataPacket, sizeof(dataPacket));

  delay(100);
}
