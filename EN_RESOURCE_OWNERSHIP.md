# Resource Ownership - Gadget V1 (EN) 🛡️

The ESP8266 is a single-core chip with limited resources. This project uses an "Exclusive Ownership" system to prevent hardware conflicts.

## 📡 1. WiFi Radio (Most Critical Resource)
The ESP8266 WiFi radio can only be in one stable operational mode at a time.

| Feature | WiFi Mode | Conflicts With |
| :--- | :--- | :--- |
| **RF Analyzer** | Promiscuous / Monitor | Repeater, Scan, Connect |
| **WiFi Repeater** | STA + AP (NAPT) | RF Analyzer |
| **WiFi Scanner** | STA (Scanning) | RF Analyzer |

**Golden Rule**: Before switching from RF Analyzer to WiFi Repeater (or vice versa), you **MUST** call the `.stop()` function on the currently active manager to return the radio to the `WIFI_OFF` state.

---

## 📺 2. OLED Display Buffer
The SSD1306 display uses a 1KB buffer in RAM.
- **Ownership**: Only the `DisplayManager` is allowed to write to the `display` object.
- **Conflict**: If two modules attempt to draw to the screen simultaneously, the display will flicker or become corrupted.
- **Solution**: Use a state machine to ensure only one module has access to the `render()` functions.

---

## 📂 3. Filesystem (LittleFS)
LittleFS is not *multi-threaded* on the ESP8266.
- **Ownership**: Global. However, avoid opening two different files for writing simultaneously to prevent flash memory block allocation failures.
- **Safety**: Always close the file (`.close()`) immediately after the operation is complete.

---

## 🚌 4. I2C Bus
The I2C bus is shared by the OLED, BME280, and VL53L0X.
- **Ownership**: Shared. The Wire library handles low-level arbitration.
- **Caution**: Avoid reading sensors too frequently (e.g., every 1ms) as it will freeze the I2C bus and hinder OLED screen updates. Use a minimum interval of 100ms.
