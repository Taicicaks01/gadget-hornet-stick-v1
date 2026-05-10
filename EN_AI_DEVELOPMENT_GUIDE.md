# AI Development Guide - Gadget V1 (EN) 🤖

This guide is intended for AI assistants (like Claude, GPT, or Gemini) to continue the development of the Gadget V1 firmware without needing a deep reverse-engineering of the entire codebase.

## 🏗️ Firmware Philosophy
Gadget V1 is not just an "Arduino Sketch". It is a **Non-blocking Modular OS** designed for scalability.
- **NO DELAY()**: Using `delay()` is strictly forbidden as it breaks background WiFi tasks (Repeater/NAPT).
- **Manager-Based**: Every hardware function (Display, Button, Radio) is isolated into singleton Manager classes.

---

## 🛠️ How to Add a New Feature (Step-by-Step)

If you are asked to add a new feature (e.g., "Bluetooth Scanner"):

### Step 1: Create a New Manager
Create a folder in `/lib/BluetoothManager/` containing `.h` and `.cpp`.
- Use a *singleton* pattern or an extern global object.
- Ensure there are `begin()`, `update()`, and `stop()` methods.

### Step 2: Register a New State
Add a new state in `enum SystemState` within `main.cpp`.
```cpp
enum SystemState {
    ...
    STATE_BLUETOOTH_MENU
};
```

### Step 3: Menu Registration
Update `DisplayManager::drawMainMenu` to include the new menu. Also, update the button navigation logic in `main.cpp` (case `STATE_MAIN_MENU`).

### Step 4: Implement the Lifecycle
Inside the `loop()` in `main.cpp`, add a new case:
```cpp
case STATE_BLUETOOTH_MENU:
    btManager.update(); // Background logic
    if (buttonManager.isLeftJustPressed()) {
        btManager.stop(); // CLEANUP IS MANDATORY
        currentState = STATE_MAIN_MENU;
        displayManager.drawMainMenu(menuIndex);
    }
    // Rendering logic with millis() timer
    break;
```

---

## ⚠️ Critical Resource Ownership Rules

### 📡 WiFi Subsystem
The ESP8266 has only one radio interface.
- **RF Analyzer** uses Promiscuous Mode.
- **WiFi Repeater** uses STA+AP mode.
- Both **MUST NOT** be active at the same time. AI must always call `.stop()` on the active module before starting another radio module.

### 📺 OLED Rendering
- Use `displayManager` for all visual output.
- Avoid `display.clearDisplay()` inside fast loops. Use *Partial Redraw* or `millis()` timers.

---

## 📂 LittleFS Structure
Use organized paths:
- `/config/` : System settings.
- `/logs/` : Sensor/RF data.
- `/assets/` : Images/Icons (if not in PROGMEM).

---

## 🤖 Message for Future AI
1. **Always check `config.h`** for pinouts.
2. **Never block the loop**. If a task takes a long time, use an internal state-machine within that module.
3. **Respect Memory**. Use the `F()` macro for all static strings.
4. **Cleanup on Exit**. Always power down unused hardware when exiting a module.
