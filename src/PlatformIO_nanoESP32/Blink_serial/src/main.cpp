#include <Arduino.h>

const int LED = 21; // Built-in LED pin for Nano ESP32

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  Serial.println("Starting Blink Program...");
}

void loop() {
  digitalWrite(LED, HIGH);
  Serial.println("LED is ON");
  delay(500);

  digitalWrite(LED, LOW);
  Serial.println("LED is OFF");
  delay(500);
}