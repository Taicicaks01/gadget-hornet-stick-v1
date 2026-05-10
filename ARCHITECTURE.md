# Architecture & Resource Management - Gadget V1 🏗️

Dokumen ini menjelaskan desain sistem internal, alur kendali, dan manajemen sumber daya hardware.

---

## 1. State Machine System 🕹️

Seluruh operasional firmware dikelola oleh satu **Global State Machine** di `main.cpp`. Hal ini menjamin tidak ada dua aplikasi yang berjalan bersamaan yang dapat menyebabkan tabrakan resource atau memori *overflow*.

### Diagram Transisi State (Abstrak)

```text
 [ BOOT ] 
    |
 [ NORMAL / DASHBOARD ] <----------------+
    |                                    |
 [ MAIN MENU ] --------------------------+
    |
    +--> [ RULER ]
    +--> [ IR TOOLS ] --> [ CLONE / MANAGE ]
    +--> [ RF ANALYZER ] --> [ LIVE / HEATMAP / etc ]
    +--> [ WIFI REPEATER ] --> [ MONITOR / CONFIG / etc ]
```

---

## 2. WiFi Ownership System 🛡️

Salah satu tantangan terbesar pada ESP8266 adalah penggunaan radio WiFi. Radio tidak bisa berada dalam mode *Promiscuous* (untuk Analyzer) dan mode *AP+STA* (untuk Repeater) secara bersamaan tanpa menyebabkan instabilitas.

### Alur Perpindahan Kepemilikan:
1.  **Request App**: Saat user memilih menu "WiFi Repeater".
2.  **Resource Cleanup**: `rfManager.stop()` dipanggil untuk memastikan Promiscuous mode mati.
3.  **App Init**: `repeaterManager.startRepeater()` mengambil alih radio.
4.  **Exit**: Saat user menekan tombol "Back" (Left), `repeaterManager.stopRepeater()` dipanggil yang melakukan `WiFi.disconnect()` dan `WiFi.mode(WIFI_OFF)`.
5.  **Radio Free**: Radio kini bebas dan siap diambil alih oleh aplikasi lain.

---

## 3. Modular Manager Pattern 🧩

Setiap modul hardware memiliki *Manager* class tersendiri:
- **`DisplayManager`**: Singleton yang mengelola buffer SSD1306. Semua penggambaran UI harus melalui class ini.
- **`ButtonManager`**: Mengelola debouncing dan deteksi *JustPressed* / *Hold* untuk 5 tombol navigasi.
- **`WiFiRepeaterManager`**: Mengelola LwIP NAPT, DNS Proxy, dan Heuristic Traffic.
- **`RFAnalyzerManager`**: Mengelola filter paket tingkat rendah (Promiscuous mode) dan channel hopping.

---

## 4. Lifecycle Management ♻️

Untuk setiap aplikasi modular, pola berikut harus diikuti:

| Stage | Lokasi | Tanggung Jawab |
| :--- | :--- | :--- |
| **Enter** | `case STATE_X:` (Triggered) | Inisialisasi sensor/radio, reset counter, draw UI awal. |
| **Update** | `loop()` | Kalkulasi data sensor, background tasks (DNS handling, packet counting). |
| **Render** | `loop()` | Update tampilan OLED jika ada perubahan data (Non-blocking). |
| **Exit** | `if(btn_left)` | Matikan sensor/radio, simpan config jika perlu, kembali ke menu. |

---

## 5. Non-blocking Design Philosophy 🚀

Aplikasi ini **TIDAK BOLEH** menggunakan `delay()`.
- **Timing**: Gunakan `millis()` untuk interval pembaruan (misal: refresh dashboard tiap 1 detik).
- **Yielding**: Selalu panggil `yield()` atau `delay(0)` pada operasi loop yang panjang agar sistem WiFi background tetap stabil (mencegah WDT Reset).
