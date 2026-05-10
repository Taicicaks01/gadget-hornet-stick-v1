# Performance Guide - Gadget V1 (EN) 🚀

Strategies to maintain maximum performance on the limited ESP8266 hardware.

## 🧠 RAM Optimization (80KB)
- **F() Macro**: Use `F("String")` for every static text string so it is stored in Flash (4MB), not RAM.
- **Heap Monitoring**: Call `ESP.getFreeHeap()` during development. If this number continuously decreases, check for memory leaks.
- **Static Objects**: Use global or singleton objects instead of creating local objects within frequently called functions.

## ⚡ CPU & Loop Timing
- **Millis Timing**: Use this pattern instead of `delay()`:
  ```cpp
  if (millis() - lastUpdate > 1000) {
      lastUpdate = millis();
      // Logic here
  }
  ```
- **Yielding**: Call `yield()` or `delay(0)` when performing intensive loops (e.g., processing large LittleFS data) to prevent disconnecting the background WiFi system and triggering a WDT reset.

## 📺 OLED Performance
- **Partial Refresh**: Only update screen areas where the values change.
- **I2C Speed**: Ensure `Wire.setClock(400000)` is called to maximize I2C bandwidth.
- **Minimize display.display()**: Sending a 1KB buffer to the screen takes ~30-40ms. Do not call it more than 20 times per second.

## 📡 WiFi Efficiency
- **Power Save**: Turn off the radio (`WiFi.mode(WIFI_OFF)`) when radio features are not in use (e.g., while in the Dashboard or Ruler menus) to save battery and reduce chip heat.
- **Background Tasks**: Be aware that NAPT and DNS Proxy run in the background. Do not overload the CPU with heavy calculations while Repeater mode is active.

## 📂 LittleFS Efficiency
- **Read-Heavy**: LittleFS is optimized for reading. Avoid frequent writing to flash as it shortens hardware lifespan and causes UI lag.
- **Batching**: If you must write a lot of data (e.g., RF Logger), collect it in a RAM buffer first before writing a large block to the file.
