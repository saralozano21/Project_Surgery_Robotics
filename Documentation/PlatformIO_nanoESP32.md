
# 💡 Arduino Nano ESP32 Blink Project with Serial Monitor (PlatformIO)

This guide shows how to create a simple Blink project for the Arduino Nano ESP32 using PlatformIO and the Arduino framework in Visual Studio Code. It also prints the LED state to the Serial Monitor.

---

## ⚙️ Project Structure

```
PlatformIO_nanoESP32/
├── src/
│   └── main.cpp
├── platformio.ini
└── PlatformIO_nanoESP32.md
```

---

## 📄 main.cpp

```cpp
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
```

---

## ⚙️ platformio.ini

```ini
[env:nanoesp32]
platform = espressif32
board = arduino_nano_esp32
framework = arduino
monitor_speed = 115200
```

---

## 🧪 How to Use

1. Open Visual Studio Code
2. Install the PlatformIO IDE extension
3. Create a new project and replace the contents with the files above
4. Connect your Arduino Nano ESP32 via USB
5. Click the **Upload** button to flash the code
6. Open the **Serial Monitor** to view LED status messages

---

## 🔌 USB Driver (if needed)

If your board is not recognized:
- Install the **Arduino CDC driver** from: [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software)

---

Happy coding with your Nano ESP32! 🚀
