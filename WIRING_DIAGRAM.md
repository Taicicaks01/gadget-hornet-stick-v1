# Wiring Diagram - Gadget V1 🔌

Skema koneksi antara Wemos D1 Mini dan seluruh modul sensor/display.

---

## 🎨 Visual Wiring (ASCII)

```text
                                  +-----------+
                                  |   OLED    |
                                  | (SSD1306) |
                                  +-----------+
                                    | | | |
               +--------------------+ | | |
               |      +---------------+ | |
               |      |      +----------+ |
               |      |      |      +-----+
               |      |      |      |
        [3V3] [SDA]  [SCL]  [GND]  [5V]
          |     |      |      |     |
    +-----+-----+------+------+-----+-----+
    |                                     |
    |           WEMOS D1 MINI             |
    |                                     |
    +-----+-----+------+------+-----+-----+
          |     |      |      |     |
        [D0]   [D5]   [D6]   [D7]  [D8]
          |     |      |      |     |
        [UP]  [IR_RX] [DOWN] [OK] [IR_TX]
```

---

## 📋 Pin Mapping Table

| Wemos Pin | GPIO | Komponen | Deskripsi |
| :--- | :--- | :--- | :--- |
| **D1** | 5 | I2C SCL | Clock untuk BME280, VL53L0X, OLED |
| **D2** | 4 | I2C SDA | Data untuk BME280, VL53L0X, OLED |
| **D5** | 14 | IR Receiver | Input sinyal IR (TSOP) |
| **D8** | 15 | IR Sender | Output PWM untuk LED IR |
| **D0** | 16 | Button UP | Navigasi (Ext Pull-up) |
| **D6** | 12 | Button DOWN | Navigasi (Int Pull-up) |
| **D7** | 13 | Button OK | Select (Int Pull-up) |
| **D3** | 0 | Button LEFT | Back (Int Pull-up) |
| **D4** | 2 | Button RIGHT | Next (Int Pull-up) |

---

## ⚠️ Hardware Notes
- **Power**: Sebaiknya gunakan regulator 3.3V eksternal jika menggunakan banyak klien pada WiFi Repeater untuk menjaga stabilitas tegangan.
- **IR Sender**: Gunakan transistor (NPN/PNP) sebagai driver LED IR, jangan menghubungkan LED IR langsung ke pin GPIO D8 karena arus terbatas.
