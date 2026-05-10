# Filesystem Structure - Gadget V1 📂

Gadget V1 menggunakan **LittleFS** sebagai sistem penyimpanan internal pada Flash Memory. Berikut adalah peta penggunaan foldernya.

---

## 🏗️ Folder Tree

```text
 /
 ├── config/            # Folder konfigurasi sistem
 │   ├── ap_config.cfg  # Pengaturan WiFi AP (SSID, Pass, Hide, Sec)
 │   ├── net_0.cfg      # Profil jaringan tersimpan slot 1
 │   └── net_1.cfg      # Profil jaringan tersimpan slot 2
 ├── ir/                # Folder penyimpanan kode IR
 │   ├── remote_tv.ir   # Data rekaman IR mentah (Raw/Hex)
 │   └── ac_living.ir
 └── rf/                # Folder log data radio
     ├── log_0.txt      # Log aktivitas paket RF
     └── heatmap_bk.dat # Backup data heatmap
```

---

## 📄 File Format Details

### 1. `ap_config.cfg`
File teks sederhana dengan baris baru sebagai pemisah.
```text
HORNET-REPEATER    # SSID Name
12345678           # Password
1                  # Security Mode (1=WPA2, 0=OPEN)
0                  # Hidden SSID (1=ON, 0=OFF)
```

### 2. `net_X.cfg` (Saved Networks)
Menyimpan kredensial WiFi upstream.
```text
WiFi-Rumah-Lantai-1
password_rahasi_123
```

---

## 🛠️ Management Logic

- **Initialization**: Dilakukan di `setup()` menggunakan `LittleFS.begin()`.
- **Formatting**: Jika filesystem rusak, sistem akan otomatis melakukan format ulang (Safe Fallback).
- **Quota**: Direkomendasikan menyisakan minimal 128KB ruang kosong agar operasi tulis file log RF tidak gagal.
