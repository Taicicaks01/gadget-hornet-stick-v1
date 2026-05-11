# Architecture Overview - Gadget V1 (ID) 🏗️

Dokumen ini menjelaskan struktur fundamental Gadget V1 Modular OS.

## 📐 Arsitektur Modular OS
Gadget V1 menggunakan arsitektur **Manager-Singleton**. Setiap komponen hardware atau fitur besar dibungkus dalam sebuah "Manager" yang bertanggung jawab atas inisialisasi, pembaruan logis, dan pembersihan resource.

### Diagram Blok Arsitektur
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

## 🔄 Alur Event & Update
Sistem ini sepenuhnya digerakkan oleh event yang diperiksa pada setiap iterasi loop:
1. **Input Stage**: `ButtonManager` memeriksa status tombol fisik.
2. **Logic Stage**: Berdasarkan `SystemState`, manager yang aktif menjalankan fungsi `update()`.
3. **Rendering Stage**: `DisplayManager` mengirimkan data ke buffer OLED jika ada perubahan visual yang terdeteksi.

---

## 🛡️ Shared Infrastructure
Beberapa resource digunakan bersama oleh banyak modul:
- **I2C Bus**: Digunakan oleh OLED, BME280, dan VL53L0X. Kita menggunakan library Wire standar.
- **Filesystem**: LittleFS digunakan untuk menyimpan konfigurasi (JSON/Text).
- **WiFi Radio**: Resource paling kritis. Hanya boleh dimiliki oleh satu manager dalam satu waktu (Exclusive Ownership).

---

## 🕹️ Mengapa State Machine?
Kami menggunakan State Machine karena:
1. **Determinisme**: Kita tahu persis apa yang sedang dijalankan oleh CPU.
2. **RAM Efficiency**: Hanya modul yang relevan yang aktif, mengurangi penggunaan heap memory.
3. **Navigasi Mudah**: Mempermudah implementasi tombol "Back" dan menu berlapis.

---

## 🚦 Core System Expansion
Untuk meningkatkan skala dan modularitas, proyek menambahkan beberapa infrastruktur inti:
- **EventManager**: Bus event ringan untuk publish/subscribe antar modul tanpa coupling langsung.
- **TaskScheduler**: Scheduler millis()-based untuk periodic/delayed tasks non-blocking.
- **AppManager**: Registry aplikasi dan lifecycle switcher untuk menjaga ownership resource.

Komponen ini membantu memastikan kode tetap terstruktur saat fitur bertambah.
