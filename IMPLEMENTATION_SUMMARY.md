# Implementation Summary - Gadget V1 🛠️

Dokumen ini merangkum detail teknis implementasi firmware Gadget V1.

---

## 🏗️ Technical Stack
- **Framework**: Arduino Framework for ESP8266.
- **Build System**: PlatformIO.
- **Storage**: LittleFS (Flash File System).
- **Architecture**: Singleton-based Manager Classes with Global State Machine.

---

## 🔧 Core Components

### 1. State Machine Control
Dikendalikan di `main.cpp` menggunakan `switch(currentState)`. Setiap state memiliki logika input (ButtonManager) dan logika output (DisplayManager) yang terpisah.

### 2. Driver Layer
- **IRremoteESP8266**: Digunakan untuk decoding sinyal IR mentah menjadi format hex dan protokol.
- **Adafruit BusIO**: Digunakan sebagai fondasi komunikasi I2C untuk sensor-sensor (BME280, VL53L0X).

### 3. Network Stack (NAPT)
Implementasi WiFi Repeater menggunakan fitur internal SDK LwIP.
- **NAT**: Memungkinkan satu IP publik (STA) digunakan oleh banyak klien (AP).
- **DNS Forwarding**: Mengarahkan DNS request klien ke upstream DNS server (8.8.8.8) secara dinamis.

---

## 📉 Resource Constraints Handling

### Heap Memory
- Seluruh teks UI menggunakan `PROGMEM` via makro `F()`.
- Objek besar (Display Buffer) dialokasikan secara statis.

### CPU Efficiency
- Task-task berat (seperti scanning WiFi) dilakukan secara sinkronis namun dengan UI feedback "Scanning...".
- Update loop berjalan pada ~20-30 FPS untuk menjaga efisiensi daya dan CPU.

---

## 📂 Project Organization
```text
 /include   -> Header konfigurasi pin (config.h)
 /lib       -> Modular Managers (Reusable components)
 /src       -> Entry point & State machine logic
 /data      -> LittleFS Initial data
```

---

## ✅ Implementation Status (v2.0)
- [x] Boot Animation & Dashboard
- [x] I2C Sensor Suite (BME280 & VL53L0X)
- [x] IR Cloning & Transmitting
- [x] 2.4GHz Packet Sniffing & Visualizing
- [x] WiFi NAPT Repeater with UI Monitor
- [x] Virtual Keyboard for SSID/Pass Entry
