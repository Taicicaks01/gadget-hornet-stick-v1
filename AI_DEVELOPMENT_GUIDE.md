# AI Development Guide - Gadget V1 (ID) 🤖

Panduan ini ditujukan bagi asisten AI (seperti Claude, GPT, atau Gemini) untuk melanjutkan pengembangan firmware Gadget V1 tanpa perlu melakukan analisis mendalam terhadap seluruh basis kode.

## 🏗️ Filosofi Firmware
Gadget V1 bukan sekadar "Arduino Sketch". Ini adalah **Non-blocking Modular OS** yang dirancang untuk skalabilitas.
- **NO DELAY()**: Penggunaan `delay()` dilarang keras karena akan merusak background tasks WiFi (Repeater/NAPT).
- **Manager-Based**: Setiap fungsi hardware (Display, Button, Radio) diisolasi ke dalam class Manager singleton.

---

## 🛠️ Cara Menambahkan Fitur Baru (Step-by-Step)

Jika Anda diminta menambahkan fitur baru (misal: "Bluetooth Scanner"):

### Langkah 1: Buat Manager Baru
Buat folder di `/lib/BluetoothManager/` berisi `.h` dan `.cpp`.
- Gunakan pola *singleton* atau objek global extern.
- Pastikan ada metode `begin()`, `update()`, dan `stop()`.

### Langkah 2: Daftarkan State Baru
Tambahkan state di `enum SystemState` dalam `main.cpp`.
```cpp
enum SystemState {
    ...
    STATE_BLUETOOTH_MENU
};
```

### Langkah 3: Registrasi Menu
Update `DisplayManager::drawMainMenu` untuk menyertakan menu baru. Update juga logika navigasi tombol di `main.cpp` (case `STATE_MAIN_MENU`).

### Langkah 4: Implementasi Lifecycle
Di dalam `loop()` pada `main.cpp`, tambahkan case baru:
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

## ⚠️ Aturan Penting Kepemilikan Resource

### 📡 WiFi Subsystem
ESP8266 hanya punya satu radio.
- **RF Analyzer** menggunakan Promiscuous Mode.
- **WiFi Repeater** menggunakan mode STA+AP.
- **TIDAK BOLEH** keduanya aktif bersamaan. AI harus selalu memanggil `.stop()` pada modul aktif sebelum memulai modul radio lainnya.

### 📺 Rendering OLED
- Gunakan `displayManager` untuk seluruh output visual.
- Hindari `display.clearDisplay()` di dalam loop cepat. Gunakan *Partial Redraw* atau timer `millis()`.

---

## 📂 Struktur LittleFS
Gunakan path yang terorganisir:
- `/config/` : Pengaturan sistem.
- `/logs/` : Data sensor/RF.
- `/assets/` : Gambar/Icon (jika tidak di PROGMEM).

---

## 🤖 Pesan untuk AI Masa Depan
1. **Always check `config.h`** untuk pinout.
2. **Never block the loop**. Jika butuh waktu lama, gunakan state-machine internal di dalam modul tersebut.
3. **Respect Memory**. Gunakan `F()` macro untuk string.
4. **Cleanup on Exit**. Selalu matikan hardware yang tidak dipakai saat keluar modul.
