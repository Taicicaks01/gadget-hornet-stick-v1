# Resource Management - Gadget V1 🛡️

Dokumen ini menjelaskan bagaimana firmware mengelola hardware yang saling berebut sumber daya (Resource Contention).

---

## 📡 WiFi Radio Ownership

ESP8266 hanya memiliki **satu jalur radio fisik** (SISO). Ini berarti radio tidak bisa melakukan dua hal yang bertolak belakang secara bersamaan.

### Konflik Utama: Analyzer vs Repeater
- **RF Analyzer**: Membutuhkan `Promiscuous Mode` (mendengarkan semua paket tanpa filter MAC).
- **WiFi Repeater**: Membutuhkan mode `STA` (terhubung ke router) dan `AP` (memancarkan sinyal).

### Mekanisme Handshake Resource:
1.  **State Check**: Saat transisi menu, sistem memeriksa modul mana yang sedang aktif.
2.  **Graceful Stop**: 
    - Sebelum masuk mode Repeater, panggil `rfManager.stop()`.
    - Sebelum masuk mode Analyzer, panggil `repeaterManager.stopRepeater()`.
3.  **Radio Re-init**: Modul yang baru mengambil alih akan melakukan inisialisasi ulang mode WiFi (`WiFi.mode()`).

---

## ⌨️ User Input Management (Buttons)

Untuk mencegah satu tombol memicu dua aksi di state yang berbeda, Gadget V1 menggunakan `ButtonManager` dengan logika *Event-Based*.

- **JustPressed**: Aksi hanya dipicu satu kali saat tombol ditekan.
- **IsPressed**: Digunakan untuk fitur seperti "Hold OK" pada Virtual Keyboard.
- **Locking**: Saat masuk ke Virtual Keyboard, navigasi Menu Utama dikunci total.

---

## 💾 Storage Management (LittleFS)

Membaca LittleFS di ESP8266 menggunakan CPU cycle yang cukup besar.

- **Caching**: Konfigurasi `ap_config.cfg` dimuat ke RAM (`apConfig` struct) saat boot. Penulisan ke file hanya dilakukan saat user memilih "Save & Apply".
- **Logging**: RF Logger menulis data secara berkala (Buffer then Write) untuk meminimalisir lag pada grafik UI.

---

## 📺 Display Management (OLED)

Display I2C memiliki kecepatan transfer data terbatas (biasanya 400kHz).

- **FPS Capping**: Refresh rate dibatasi menggunakan `lastDisplayUpdate`.
- **Minimalist Drawing**: Menggunakan fungsi primitif `Adafruit_GFX` daripada gambar bitmap besar untuk menghemat RAM dan waktu transfer.
