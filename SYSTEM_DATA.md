# System Data & Structure - Gadget V1 📂

Dokumen ini berisi detail teknis tentang wiring hardware, struktur penyimpanan file (LittleFS), dan struktur menu navigasi.

---

## 1. Wiring & Pinout (Wemos D1 Mini) 🔌

| Fungsi | Pin (Wemos) | GPIO | Keterangan |
| :--- | :--- | :--- | :--- |
| **I2C SDA** | D2 | GPIO 4 | BME280, VL53L0X, OLED |
| **I2C SCL** | D1 | GPIO 5 | BME280, VL53L0X, OLED |
| **IR Receiver** | D5 | GPIO 14 | TSOP 38kHz |
| **IR Sender** | D8 | GPIO 15 | IR LED (Power via Transistor) |
| **Button UP** | D0 | GPIO 16 | External Pull-up required |
| **Button DOWN** | D6 | GPIO 12 | Internal Pull-up |
| **Button OK** | D7 | GPIO 13 | Internal Pull-up |
| **Button LEFT** | D3 | GPIO 0 | Internal Pull-up |
| **Button RIGHT**| D4 | GPIO 2 | Internal Pull-up |

---

## 2. LittleFS File Structure 📁

Sistem menggunakan LittleFS untuk menyimpan konfigurasi dan log. Berikut adalah struktur direktorinya:

```text
 / (root)
 ├── config/
 │   ├── ap_config.cfg      # SSID, Pass, Sec mode untuk AP Repeater
 │   └── net_0.cfg          # Saved Network Slot 0 (SSID, Pass)
 │   └── net_1.cfg          # Saved Network Slot 1
 ├── ir/
 │   ├── tv_samsung.ir      # Contoh file rekaman kode IR
 │   └── ac_panasonic.ir
 └── rf/
     ├── scan_log_0.txt     # Log aktivitas paket RF
     └── heatmap.dat        # Data heatmap spektrum terakhir
```

---

## 3. Full Menu Tree 🗺️

Navigasi antar menu menggunakan tombol UP/DOWN untuk memilih dan OK/RIGHT untuk masuk. LEFT untuk kembali (Back).

```text
MAIN MENU
 ├── 1. ENV Dashboard      (Direct Display)
 ├── 2. Smart Ruler        (Direct Display)
 ├── 3. Universal IR Tools
 │    ├── Clone IR         (Capture -> Send)
 │    └── Manage IR        (File Browser -> Send)
 ├── 4. 2.4GHz Analyzer
 │    ├── Live RF          (Graph PPS)
 │    ├── Spike Detector   (Threshold Alarm)
 │    ├── Heatmap          (Channel graph)
 │    ├── RF Radar         (Signal tracking)
 │    ├── RF Logger        (Save to File)
 │    └── Log Viewer       (Browse Logs)
 └── 5. WiFi Repeater
      ├── Start Repeater   (Run with Auto-load Slot 0)
      ├── Saved Networks   (Browse -> Load / Delete)
      ├── Repeater Config  (SSID, Pass, Security, Hidden)
      └── Scan Networks    (Interactive List -> Keyboard)
```

---

## 4. I2C Addresses 🛰️

| Device | Address | Keterangan |
| :--- | :--- | :--- |
| **SSD1306** | `0x3C` | OLED Display |
| **BME280** | `0x76` | Env Sensor (Bisa 0x77 tergantung module) |
| **VL53L0X** | `0x29` | ToF Sensor (Default) |
