# Module Lifecycle - Gadget V1 (EN) ♻️

This document explains the lifecycle of each module or feature in Gadget V1.

## 🔄 Standard Lifecycle Pattern

Every feature that has its own state must follow a 4-phase pattern:

### 1. Initialization (`onEnter` / Triggered)
Executed during the state transition in `main.cpp`.
- **Tasks**: Allocate resources (e.g., call `rfManager.start()`), reset counters, and draw the initial UI.
- **Notes**: This phase runs **only once** upon entering the feature.

### 2. Processing (`update()`)
Called every time the loop runs as long as that state is active.
- **Tasks**: Read sensors, process WiFi packets, handle specific button navigation.
- **Notes**: Must be **non-blocking** (no long loops or `delay`).

### 3. Visualization (`render()`)
Usually part of the `update` or a loop timer.
- **Tasks**: Send data to the OLED buffer.
- **Notes**: Use a time interval (e.g., every 500ms) so the CPU is not overloaded with continuous rendering.

### 4. Cleanup (`onExit` / Cleanup)
Executed when the user presses the "Back" button (Left).
- **Tasks**: Turn off the WiFi radio, release sensors, save configuration to LittleFS.
- **Notes**: **MANDATORY**. Failure to perform cleanup will cause crashes or the failure of other modules (especially WiFi modules).

---

## 📋 Example Cases

### WiFi Repeater Lifecycle
1. **Enter**: Load config from Flash, initialize AP+STA mode.
2. **Update**: Execute background NAT routing and DNS proxy.
3. **Exit**: Call `stopRepeater()`, turn off the radio completely, return to the main menu.

### RF Analyzer Lifecycle
1. **Enter**: Call `sniffer_start()`, enable *Promiscuous Mode*.
2. **Update**: Calculate packets per second (PPS) and perform *Channel Hopping*.
3. **Exit**: Call `sniffer_stop()`, set WiFi back to OFF mode.
