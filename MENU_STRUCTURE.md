# Menu Structure - Gadget V1 🗺️

Navigasi antarmuka Gadget V1 didesain agar intuitif dengan navigasi 5 tombol.

---

## 🌳 Full Menu Hierarchy

### [ MAIN MENU ]
- **1. ENV Dashboard** (Informasi sensor BME280)
- **2. Smart Ruler** (Jarak VL53L0X)
- **3. Universal IR Tools**
    - ├── **Clone IR** (Capture sinyal baru)
    - └── **Manage IR** (Kirim sinyal tersimpan)
- **4. 2.4GHz Analyzer**
    - ├── **Live RF** (Grafik trafik)
    - ├── **Spike Detector** (Alarm sinyal kuat)
    - ├── **Heatmap** (Kepadatan kanal)
    - ├── **RF Radar** (Tracking sumber sinyal)
    - ├── **RF Logger** (Catat ke file)
    - └── **Log Viewer** (Baca file log)
- **5. WiFi Repeater**
    - ├── **Start Repeater** (Aktifkan router)
    - ├── **Saved Networks** (Daftar WiFi tersimpan)
    - ├── **Repeater Config** (Ubah nama/pass AP)
    - └── **Scan Networks** (Cari WiFi baru)
- **6. Onscreen Display**
    - ├── **Custom Text**
    - ├── **Running Text**
    - └── **Eye Animation**
- **7. Settings**
    - └── **Screen Sleep**
- **8. System Info**
    - ├── **Free RAM**
    - ├── **Flash Usage**
    - └── **Uptime / WiFi Status**
- **9. File Explorer**
    - ├── **Browse /config, /logs, /ir, /rf, /repeater**
    - ├── **View file info**
    - └── **Delete / Preview (line-by-line)**

---

## 🕹️ Button Control Logic

| Tombol | Menu Context | App Context (e.g. Keyboard) |
| :--- | :--- | :--- |
| **UP** | Pilih item sebelumnya | Pindah baris atas |
| **DOWN** | Pilih item berikutnya | Pindah baris bawah |
| **LEFT** | Kembali ke Menu/Normal | Hapus karakter (Backspace) |
| **RIGHT**| Masuk ke Sub-menu | Pindah karakter ke kanan |
| **OK** | Masuk/Eksekusi | Pilih karakter (Tap) / Enter (Hold) |

### [ ONSCREEN DISPLAY ]
- **LEFT**: Kembali ke menu sebelumnya, atau hapus karakter saat editor aktif.
- **OK**: Masuk ke submenu, insert karakter, atau pause/resume pada running text fullscreen.
- **RIGHT**: Keluar dari fullscreen running text, atau pindah fokus / select pada mode tertentu.
- **UP/DOWN**: Navigasi menu, atau ubah parameter animasi di mode aktif.

### [ RUNNING TEXT FULLSCREEN ]
- Teks berjalan kanan → kiri pada area OLED penuh.
- UI lain disembunyikan untuk menjaga tampilan terminal-style yang bersih.
- `LEFT` / `RIGHT`: keluar cepat ke menu OSD.
- `OK`: pause/resume animasi.
- Refresh animasi dibatasi ke sekitar 10–20 FPS agar smooth tanpa flicker.

### [ SETTINGS -> SCREEN SLEEP ]
- **UP/DOWN**: Ubah timeout sleep.
- **OK**: Toggle enable/disable sleep.
- **LEFT**: Kembali ke Settings Menu.

---

## 📱 Virtual Keyboard Layout

Keyboard menggunakan grid 3x12:
- **Baris 1**: A B C D E F G H I J K L
- **Baris 2**: M N O P Q R S T U V W X
- **Baris 3**: Y Z 0 1 2 3 4 5 6 7 8 9
