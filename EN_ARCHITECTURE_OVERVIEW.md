# Architecture Overview - Gadget V1 (EN) 🏗️

This document explains the fundamental structure of the Gadget V1 Modular OS.

## 📐 Modular OS Architecture
Gadget V1 uses a **Manager-Singleton** architecture. Each hardware component or major feature is wrapped in a "Manager" responsible for initialization, logical updates, and resource cleanup.

### Architectural Block Diagram
```text
 +---------------------------------------+
 |             MAIN LOOP (OS)            |
 | (Switch-Case Global State Machine)    |
 +---------+------------+------------+---+
           |            |            |
   [Managers Layer]     |    [Hardware Layer]
   +----------------+   |    +---------------+
   | ButtonManager  |---+--->| 5-Way Buttons |
   +----------------+   |    +---------------+
   | DisplayManager |---+--->| OLED SSD1306  |
   +----------------+   |    +---------------+
   | SensorManager  |---+--->| BME280/ToF    |
   +----------------+   |    +---------------+
   | WiFiManager    |---+--->| ESP8266 Radio |
   +----------------+        +---------------+
```

---

## 🔄 Event & Update Flow
The system is entirely event-driven, checked at every loop iteration:
1. **Input Stage**: `ButtonManager` checks the status of the physical buttons.
2. **Logic Stage**: Based on the `SystemState`, the active manager runs its `update()` function.
3. **Rendering Stage**: `DisplayManager` sends data to the OLED buffer if a visual change is detected.

---

## 🛡️ Shared Infrastructure
Several resources are shared by many modules:
- **I2C Bus**: Used by OLED, BME280, and VL53L0X. We use the standard Wire library.
- **Filesystem**: LittleFS is used to store configurations (JSON/Text).
- **WiFi Radio**: The most critical resource. It can only be owned by one manager at a time (Exclusive Ownership).

---

## 🕹️ Why a State Machine?
We use a State Machine because:
1. **Determinism**: We know exactly what the CPU is currently executing.
2. **RAM Efficiency**: Only relevant modules are active, reducing heap memory usage.
3. **Easy Navigation**: Simplifies the implementation of the "Back" button and multi-layered menus.
