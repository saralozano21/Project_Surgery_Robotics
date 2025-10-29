#include <WiFi.h>
#include <WiFiUdp.h>
#include "MPU9250.h"
#include <Wire.h> // Needed for I2C to read IMU
#include <ArduinoJson.h> // Compatible amb versió 7.4.2

// Device ID
const char *deviceId = "G5_Gri";

// Wi-Fi credentials
const char *ssid = "Robotics_UB";
const char *password = "rUBot_xx";

// Vibration motor settings
const int vibrationPin = 23; // Pin for the vibration motor

// Botons
const int PIN_S1 = 14;
const int PIN_S2 = 27;
int s1Status = HIGH;
int s2Status = HIGH;

// UDP settings
IPAddress receiverESP32IP(192, 168, 1, 53); // IP of receiver ESP32
IPAddress receiverComputerIP(192, 168, 1, 55); // IP of PC
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
  }
  s1Status = digitalRead(PIN_S1);
  s2Status = digitalRead(PIN_S2);
}

void sendOrientationUDP() {
  JsonDocument doc;
  doc["device"] = deviceId;
  doc["roll"] = Gri_roll;
  doc["pitch"] = Gri_pitch;
  doc["yaw"] = Gri_yaw;
  doc["s1"] = s1Status;
  doc["s2"] = s2Status;

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer, sizeof(jsonBuffer));

  // Send to ESP32 Servos
  udp.beginPacket(receiverESP32IP, udpPort);
  udp.write((const uint8_t*)jsonBuffer, strlen(jsonBuffer));
  udp.endPacket();

  // Send to Computer
  udp.beginPacket(receiverComputerIP, udpPort);
  udp.write((const uint8_t*)jsonBuffer, strlen(jsonBuffer));
  udp.endPacket();
}

// Added improvement: To receive torques
void receiveTorquesUDP() {
  int packetSize = udp.parsePacket(); // Asks UDP if there's any packet available
  if (packetSize) {
    char incomingPacket[512]; // 512 bytes to keep the UDP message
    int len = udp.read(incomingPacket, sizeof(incomingPacket) - 1); // Reads the message
    if (len > 0) {
      incomingPacket[len] = 0; 
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, incomingPacket);
    if (!error) {
      // Read received torques
      if (doc.containsKey("Torque_roll1")) Torque_roll1 = doc["Torque_roll1"];
      if (doc.containsKey("Torque_pitch")) Torque_pitch = doc["Torque_pitch"];
      if (doc.containsKey("Torque_yaw")) Torque_yaw = doc["Torque_yaw"];


      // Vibration motor control based on torque values
      float totalTorque = Torque_roll1 + Torque_pitch + Torque_yaw;
      // Convert torque to PWM value (0-255)
      int vibrationValue = constrain(totalTorque * 2.5, 0, 255); // Adjust the scaling factor as needed
      ledcWrite(0, vibrationValue); // Set the PWM value for the vibration motor
      Serial.print("Vibration motor value: ");
      Serial.println(vibrationValue); 
    }
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  delay(2000);

  if (!mpu.setup(0x68)) {
    while (1) {
      Serial.println("MPU connection failed.");
      delay(5000);
    }
  }
  Serial.println("MPU connected");
  delay(2000);

  connectToWiFi();
  udp.begin(udpPort);
  Serial.println("UDP initialized");

  pinMode(PIN_S1, INPUT);
  pinMode(PIN_S2, INPUT);

  // Added improvement: To control the vibration motor
  // Configure PWM for the vibration motor (channel 0)
  ledcSetup(0, 5000, 8); // Channel 0, frequency 5kHz, resolution 8 bits
  ledcAttachPin(vibrationPin, 0); // Attach the vibration motor to channel 0
}

void loop() {
  updateOrientation();
  sendOrientationUDP();
  delay(10);
}
