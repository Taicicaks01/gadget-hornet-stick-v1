# Gadget V1 - Universal IR & Sensor Multi-tool (Modular OS) 🛠️

**Gadget V1** adalah firmware berbasis *Modular OS* untuk ESP8266 (Wemos D1 Mini) yang dirancang sebagai perangkat multi-fungsi portabel untuk analisis sinyal, pemantauan lingkungan, dan utilitas jaringan.

Dibangun dengan arsitektur *non-blocking state machine*, perangkat ini mampu menangani berbagai modul sensor dan fitur radio (IR & WiFi) secara efisien tanpa gangguan performa.

---

## 🚀 Fitur Utama

1.  **📊 ENV Dashboard**: Pemantauan real-time suhu, kelembaban, tekanan udara, dan estimasi ketinggian menggunakan sensor BME280.
2.  **📏 Smart Ruler**: Pengukur jarak presisi berbasis laser (ToF) menggunakan sensor VL53L0X.
3.  **📡 Universal IR Tools**: Kemampuan untuk melakukan cloning remote IR, pengujian protokol, dan simulasi sinyal.
4.  **📉 2.4GHz RF Analyzer**: Pemindai spektrum WiFi untuk mendeteksi paket, heatmap saluran, dan deteksi lonjakan sinyal (Spike Detection).
5.  **🔁 WiFi Repeater**: Router portabel dengan fitur NAPT (Network Address Port Translation), DNS Proxy, dan *Transfer Monitor* heuristik.

---

## 🔌 Hardware Requirements

| Komponen | Spesifikasi | Keterangan |
| :--- | :--- | :--- |
| **Microcontroller** | Wemos D1 Mini (ESP8266) | Core Processor |
| **Display** | OLED SSD1306 128x64 I2C | Antarmuka Visual |
| **Environmental Sensor** | BME280 (I2C) | Suhu, Humid, Tekanan |
| **Distance Sensor** | VL53L0X (I2C) | Laser Time-of-Flight |
| **IR Receiver** | TSOP4838 atau sejenis | Penerima Sinyal IR |
| **IR Transmitter** | IR LED + Transistor Driver | Pengirim Sinyal IR |
| **Buttons** | 5-Way Navigation Buttons | Up, Down, Left, Right, OK |

---

## 📍 Pin Mapping (Wemos D1 Mini)

```text
       [ Wemos D1 Mini ]
       +---------------+
   RST |               | TX
    A0 |               | RX
    D0 | [BTN_UP]      | D1 [I2C_SCL]
    D5 | [IR_RECV]     | D2 [I2C_SDA]
    D6 | [BTN_DOWN]    | D3 [BTN_LEFT]
    D7 | [BTN_OK]      | D4 [BTN_RIGHT]
    D8 | [IR_SEND]     | GND
    3V |               | 5V
       +---------------+
```

*Detail Pinout dapat dilihat di [SYSTEM_DATA.md](./SYSTEM_DATA.md)*

---

## 🏗️ Arsitektur Firmware

Proyek ini menggunakan pola **Modular Manager System**. Setiap fitur utama diisolasi ke dalam `Manager` class-nya masing-masing untuk menjaga efisiensi RAM dan skalabilitas.

### Lifecycle System
Setiap aplikasi mengikuti alur lifecycle yang ketat:
- **onEnter()**: Inisialisasi resource (misal: Mengaktifkan mode Promiscuous WiFi).
- **update()**: Logika non-blocking yang dipanggil setiap loop.
- **render()**: Pengiriman data ke buffer OLED.
- **onExit()**: Pembersihan resource (misal: Mematikan WiFi agar radio bebas digunakan modul lain).

---

## 🛠️ Instalasi & Build

### Prasyarat
- **VS Code** dengan ekstensi **PlatformIO**.
- Framework **Arduino** untuk ESP8266.

### Libraries Utama
- `Adafruit GFX` & `Adafruit SSD1306` (Display)
- `Adafruit BME280` & `Adafruit Unified Sensor` (Environment)
- `Adafruit VL53L0X` (Distance)
- `IRremoteESP8266` (IR Control)
- `LittleFS` (Storage)

### Cara Upload
1. Clone repositori ini.
2. Buka folder proyek di VS Code (PlatformIO).
3. Hubungkan Wemos D1 Mini via USB.
4. Jalankan perintah `Upload` dan `Upload Filesystem Image` (untuk konfigurasi LittleFS).

---

## 🗺️ Future Roadmap
- [ ] Implementasi **Settings Menu** untuk kustomisasi kecerahan dan timeout.
- [ ] Penambahan modul **Bluetooth Scanner** (Untuk ESP32 variant).
- [ ] Support **Multiple IR Profiles** dari file LittleFS.
- [ ] Optimasi **Deep Sleep Mode** untuk penghematan baterai.

---

## 📜 Lisensi & Kontributor
Proyek ini dikembangkan oleh **Wayan** sebagai bagian dari sistem Universal Tool modular. Kontribusi dan saran sangat terbuka melalui Pull Request.

---
*Dokumentasi detail lainnya:*
[Feature Docs](./FEATURE_DOCUMENTATION.md) | [Architecture](./ARCHITECTURE.md) | [System Data](./SYSTEM_DATA.md) | [Optimization](./MEMORY_OPTIMIZATION.md) | [Changelog](./CHANGELOG.md)

---
## 🤖 AI Development & Maintenance
Project ini dirancang untuk dikembangkan oleh AI asisten masa depan. Sila baca panduan teknis mendalam di bawah ini:
- **[ID] [AI Development Guide](./AI_DEVELOPMENT_GUIDE.md)**
- **[EN] [AI Development Guide](./EN_AI_DEVELOPMENT_GUIDE.md)**
- **[Full Documentation Suite Index](./AI_DEVELOPMENT_GUIDE.md)**
