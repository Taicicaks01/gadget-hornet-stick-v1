# Resource Ownership - Gadget V1 (ID) 🛡️

ESP8266 adalah chip single-core dengan sumber daya terbatas. Proyek ini menggunakan sistem "Kepemilikan Eksklusif" untuk mencegah konflik hardware.

## 📡 1. WiFi Radio (Resource Paling Kritis)
Radio WiFi ESP8266 hanya bisa berada dalam satu mode operasional yang stabil pada satu waktu.

| Fitur | Mode WiFi | Konflik Dengan |
| :--- | :--- | :--- |
| **RF Analyzer** | Promiscuous / Monitor | Repeater, Scan, Connect |
| **WiFi Repeater** | STA + AP (NAPT) | RF Analyzer |
| **WiFi Scanner** | STA (Scanning) | RF Analyzer |

**Aturan Emas**: Sebelum berpindah dari RF Analyzer ke WiFi Repeater (atau sebaliknya), Anda **WAJIB** memanggil fungsi `.stop()` pada manager yang sedang aktif untuk mengembalikan radio ke kondisi `WIFI_OFF`.

---

## 📺 2. OLED Display Buffer
Layar SSD1306 menggunakan buffer 1KB di RAM.
- **Ownership**: Hanya `DisplayManager` yang boleh menulis ke objek `display`.
- **Conflict**: Jika dua modul mencoba menggambar ke layar secara bersamaan, tampilan akan berkedip (*flicker*) atau korup.
- **Solution**: Gunakan state machine untuk memastikan hanya satu modul yang memiliki hak akses ke fungsi `render()`.

---

## 📂 3. Filesystem (LittleFS)
LittleFS tidak bersifat *multi-threaded* pada ESP8266.
- **Ownership**: Global. Namun, hindari membuka dua file berbeda untuk ditulis secara bersamaan guna mencegah kegagalan alokasi blok memori flash.
- **Safety**: Selalu tutup file (`.close()`) segera setelah operasi selesai.

---

## 🚌 4. I2C Bus
Bus I2C digunakan bersama oleh OLED, BME280, dan VL53L0X.
- **Ownership**: Shared. Library Wire menangani arbitrase tingkat rendah.
- **Caution**: Hindari pembacaan sensor yang terlalu sering (misal: tiap 1ms) karena akan membekukan bus I2C dan menghambat pembaruan layar OLED. Gunakan interval minimal 100ms.
