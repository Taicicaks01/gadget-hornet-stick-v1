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

---

## 🕹️ Button Control Logic

| Tombol | Menu Context | App Context (e.g. Keyboard) |
| :--- | :--- | :--- |
| **UP** | Pilih item sebelumnya | Pindah baris atas |
| **DOWN** | Pilih item berikutnya | Pindah baris bawah |
| **LEFT** | Kembali ke Menu/Normal | Hapus karakter (Backspace) |
| **RIGHT**| Masuk ke Sub-menu | Pindah karakter ke kanan |
| **OK** | Masuk/Eksekusi | Pilih karakter (Tap) / Enter (Hold) |

---

## 📱 Virtual Keyboard Layout

Keyboard menggunakan grid 3x12:
- **Baris 1**: A B C D E F G H I J K L
- **Baris 2**: M N O P Q R S T U V W X
- **Baris 3**: Y Z 0 1 2 3 4 5 6 7 8 9
