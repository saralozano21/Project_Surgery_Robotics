#include <WiFi.h>
#include <WiFiUdp.h>
#include "MPU9250.h"
#include <Wire.h> //needed for I2C to read IMU
#include <ArduinoJson.h>

// Device ID
const char *deviceId = "G4_Endo";

// Wi-Fi credentials
const char *ssid = "Robotics_UB";
const char *password = "rUBot_xx";

// Botons 
const int PIN_S3 = 14;
const int PIN_S4 = 27;
int s3Status = HIGH;
int s4Status = HIGH;

// UDP settings
IPAddress receiverESP32IP(192, 168, 1, 41); // IP address of the G4-Gripper ESP32 - CHANGE THIS!
IPAddress receiverComputerIP(192, 168, 1, 5); // IP address of your PC4-computer - CHANGE THIS!
const int udpPort = 12345;
WiFiUDP udp;

// MPU-9250 object
MPU9250 mpu;

// Orientation data
float Gri_roll = 0.0, Gri_pitch = 0.0, Gri_yaw = 0.0;

void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected!");
  Serial.println("IP Address: " + WiFi.localIP().toString());
  Serial.print("ESP32 MAC Address: ");
  Serial.println(WiFi.macAddress());
}

void updateOrientation() {
  if (mpu.update()) {
    Gri_yaw = -mpu.getYaw();
    Gri_pitch = -mpu.getPitch();
    Gri_roll = mpu.getRoll();
    s3Status = digitalRead(PIN_S3); // Read the state of the button
    s4Status = digitalRead(PIN_S4); // Read the state of the button
  }
}

void sendOrientationUDP() {
  StaticJsonDocument<256> doc;
  doc["device"] = deviceId;
  doc["roll"] = Gri_roll;
  doc["pitch"] = Gri_pitch;
  doc["yaw"] = Gri_yaw;
  doc["s3"] = s3Status; // Button 1 status
  doc["s4"] = s4Status; // Button 2 status

  char jsonBuffer[512];
  size_t bytes = serializeJson(doc, jsonBuffer);
    if (bytes == 0){
        Serial.println(F("Serialization Failed"));
        return;
    }

  // Send to ESP32
  udp.beginPacket(receiverESP32IP, udpPort);
  udp.write((const uint8_t*)jsonBuffer, bytes); // Cast to const uint8_t*
  udp.endPacket();

  // Send to Computer
  udp.beginPacket(receiverComputerIP, udpPort);
  udp.write((const uint8_t*)jsonBuffer, bytes); // Cast to const uint8_t*
  udp.endPacket();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();//needed for I2C to read IMU
  delay(2000);

  // Inicialitza el MPU-9250
  if (!mpu.setup(0x68)) {
    while (1) {
      Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
      delay(5000);
    }
  }
  Serial.println("MPU connected");
  delay(2000);

  // Connecta a la xarxa Wi-Fi
  connectToWiFi();

  // Comença UDP
  udp.begin(udpPort);
  Serial.println("UDP initialized.");
}

void loop() {
  updateOrientation(); // Actualitza les dades del sensor
  sendOrientationUDP(); // Envia les dades al receptor via UDP
  delay(10); // Ajusta la freqüència d'enviament si cal
}
