# Changelog - Gadget V1 📜

Seluruh perubahan besar pada proyek Gadget V1 dicatat di sini.

---

## [v2.0] - 2026-05-10
### Added
- **Modular OS Architecture**: Migrasi dari script prosedural ke sistem State Machine.
- **2.4GHz RF Analyzer**: Modul baru untuk analisis paket pasif dan heatmap.
- **WiFi Repeater Pro**:
  - Support NAPT & DNS Proxy.
  - Heuristic Traffic Monitor (Real-time RX/TX speed).
  - Multi-config storage (Hingga 5 jaringan tersimpan).
  - Advanced AP Config (Hidden SSID, Open/WPA2, SSID Name change).
- **OLED Virtual Keyboard**: Input teks interaktif tanpa membutuhkan koneksi smartphone/serial.
- **Bilingual Documentation Suite**: Dokumentasi lengkap dalam Bahasa Indonesia & English untuk pengembangan masa depan oleh AI/Manusia.
- **AI Development Guide**: Panduan teknis mendalam untuk asisten AI agar bisa meneruskan project secara modular.
- **LittleFS Management**: Sistem penyimpanan terstruktur untuk `/config`, `/ir`, dan `/rf`.
- **Resource Ownership System**: Manajemen aman untuk berbagi radio WiFi antara mode Analyzer dan Repeater.

### Improved
- **Memory Efficiency**: Implementasi makro `F()` secara menyeluruh.
- **Non-blocking Loop**: Menghilangkan seluruh fungsi `delay()` untuk stabilitas WiFi background.
- **UI UX**: Refresh desain menu dan dashboard dengan visual yang lebih premium dan responsif.

---

## [v1.0] - 2026-04-20
### Added
- **Initial Release**.
- **ENV Dashboard**: Integrasi BME280 dasar.
- **Smart Ruler**: Integrasi VL53L0X.
- **IR Clone**: Fungsi copy-paste sinyal IR dasar.
- **Main Menu**: Navigasi dasar menggunakan OLED.

---

## [Future Roadmap]
- **v2.1**: Implementasi Settings Menu (Brightness, Timeout).
- **v2.2**: Support SD Card (via SPI) untuk logging data dalam jumlah besar.
- **v3.0**: Porting ke ESP32 untuk dual-core support dan Bluetooth scanning.
