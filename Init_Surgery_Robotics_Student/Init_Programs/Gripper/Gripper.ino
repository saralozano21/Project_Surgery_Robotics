#include <WiFi.h>
#include <WiFiUdp.h>
#include "MPU9250.h"
#include <Wire.h> //needed for I2C to read IMU
#include <ArduinoJson.h>

// Device ID
const char *deviceId = "G4_Gri";

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
IPAddress receiverESP32IP(192, 168, 1, 43); // IP address of the G4-Servos ESP32 - CHANGE THIS!
IPAddress receiverComputerIP(192, 168, 1, 4); // IP address of your PC4-computer - CHANGE THIS!
const int udpPort = 12345;
WiFiUDP udp;

// MPU-9250 object
MPU9250 mpu;

// Vibration object settings
// const int vibrationPin = 23; // Pin for the vibration motor

// Variables to store received data
float Endo_roll = 0.0, Endo_pitch = 0.0, Endo_yaw = 0.0;
float Gri_roll = 0.0, Gri_pitch = 0.0, Gri_yaw = 0.0;
float Torque_roll1 = 0.0, Torque_roll2 = 0.0, Torque_pitch = 0.0, Torque_yaw = 0.0;


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
  //Sensors
  s1Status = digitalRead(PIN_S1); // Read the state of the button
  s2Status = digitalRead(PIN_S2); // Read the state of the button
}

void sendOrientationUDP() {
  StaticJsonDocument<256> doc;
  doc["device"] = deviceId;
  doc["roll"] = Gri_roll;
  doc["pitch"] = Gri_pitch;
  doc["yaw"] = Gri_yaw;
  doc["s1"] = s1Status; // Button 1 status
  doc["s2"] = s2Status; // Button 2 status

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

void receiveOrientationUDP() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    byte packetBuffer[512];
    int len = udp.read(packetBuffer, 512);
    if (len > 0) {
      packetBuffer[len] = '\0';
      //Serial.print("Received packet size: ");
      //Serial.println(packetSize);
      //Serial.print("Received: ");
      //Serial.println((char*)packetBuffer);

      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, packetBuffer);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }

      const char* device = doc["device"];
      if (strcmp(device, "G4_Endo") == 0) {
        Endo_roll = round(doc["roll"].as<float>());
        Endo_pitch = round(doc["pitch"].as<float>());
        Endo_yaw = round(doc["yaw"].as<float>());
        //Serial.print("Endo_Roll: "); Serial.print(Endo_roll); Serial.print(" Endo_Pitch: "); Serial.print(Endo_pitch); Serial.print(" Endo_Yaw: "); Serial.println(Endo_yaw);
      } else {
        //Serial.println("Unknown device.");
      }
    }
  }
}

void receiveTorquesUDP() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    byte packetBuffer[512];
    int len = udp.read(packetBuffer, 512);
    if (len > 0) {
      packetBuffer[len] = '\0';

      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, packetBuffer);
      if (error) {
        Serial.print(F("deserializeJson() failed (torques): "));
        Serial.println(error.f_str());
        return;
      }

      const char* device = doc["device"];
      if (strcmp(device, "G4_Servos") == 0) {
        Torque_roll1 = doc["t_roll1"];
        Torque_roll2 = doc["t_roll2"];
        Torque_pitch = doc["t_pitch"];
        Torque_yaw = doc["t_yaw"];
        //Serial.println("Rebut torque dels servos");
        // Imprimeix per consola
        float totalTorque = Torque_roll1 + Torque_pitch + Torque_yaw;
        //Serial.println("=== TORQUES REBUDES ===");
        //Serial.print("Torque roll1: "); Serial.println(Torque_roll1);
        //Serial.print("Torque pitch: "); Serial.println(Torque_pitch);
        //Serial.print("Torque yaw: "); Serial.println(Torque_yaw);
        Serial.print("Total Torque: "); Serial.println(totalTorque);
        //Serial.println("=========================");
        // Vibration motor control based on torque values
        // Convert torque to PWM value (0-255)
        int vibrationValue = constrain(totalTorque * 2.5, 0, 255); // Adjust the scaling factor as needed
        ledcWrite(0, vibrationValue); // Set the PWM value for the vibration motor
        Serial.print("Vibration motor value: ");
        Serial.println(vibrationValue);     
      }
    }
  }
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

  // Connect to WiFi network
  connectToWiFi();
  // Init UDP
  udp.begin(udpPort);
  Serial.println("UDP initialized");
  // Configure PWM for the vibration motor (channel 0)
  ledcSetup(0, 5000, 8); // Channel 0, frequency 5kHz, resolution 8 bits
  ledcAttachPin(vibrationPin, 0); // Attach the vibration motor to channel 0
  pinMode(PIN_S1, INPUT); // Set button pin as input
  pinMode(PIN_S2, INPUT); // Set button pin as input
}

void loop() {
  receiveOrientationUDP();
  updateOrientation(); // Update IMU and sensor data from Gripper
  sendOrientationUDP(); // Send IMU and sensor data to Servos and Computer with UDP
  receiveTorquesUDP(); // Receive torques from Servos
  delay(10);
}