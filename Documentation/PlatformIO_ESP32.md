
# 💡 ESP32 Blink Project with PlatformIO and VS Code

This project demonstrates how to blink an LED using an ESP32 board programmed with PlatformIO and the Arduino framework inside Visual Studio Code.

---

## 🧰 Requirements

- ESP32 DevKit v1
- USB cable
- Visual Studio Code
- PlatformIO IDE Extension
- Python 3.5 or higher

---

## 🚀 Create the Blink Project

- Click the alien icon (PlatformIO Home)
- Click on Projects & Configuration
- Click `New Project`
- Name the project: `ESP32Test_Blink`
- Select board: `Espressif ESP32 Dev Module`
- Framework: `Arduino`
- Click `Finish`

PlatformIO will create the project structure and download necessary tools.

---
## ⚙️ Project Structure

```
ESP32Test_Blink/
├── src/
│   └── main.cpp
├── lib
└── platformio.ini

```

---
## 🧾 Source Code: `src/main.cpp`

```cpp
#include <Arduino.h>

const int LED = 2;

void setup() {
  pinMode(LED, OUTPUT);
}

void loop() {
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
  delay(500);
}
```

---

## ⚙️ Configuration File: `platformio.ini`

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
```

---

## 🔌 Install USB Driver for ESP32

If your ESP32 is not recognized by your computer:

- Download the **CP210x USB to UART Bridge VCP Drivers** from Silicon Labs:
  [https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
- Install the driver for your operating system (Windows, macOS, Linux)
- Reconnect the ESP32 board

---

## 📤 Upload Code to ESP32

1. Connect ESP32 via USB
2. Click the green arrow (Upload) at the bottom bar
3. If upload fails, press and hold the **BOOT** button during upload

---

## 🔍 Use the Serial Monitor

1. Click the plug icon (Serial Monitor) at the bottom bar
2. Ensure `monitor_speed` is set correctly in `platformio.ini`
3. Use `Serial.println()` in your code to print messages

---

## 🎥 Recommended Video Tutorials

- [Perfect Combo for ESP32: VS Code & PlatformIO Guide](https://www.youtube.com/watch?v=WxELHnnlBmU)
- [Getting Started with PlatformIO (DroneBot Workshop)](https://www.youtube.com/watch?v=JmvMvIphMnY)

---

## 📚 Resources

- PlatformIO Documentation: [https://docs.platformio.org/](https://docs.platformio.org/)
- ESP32 USB Drivers: [https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)

---

Happy coding with ESP32! 🚀
