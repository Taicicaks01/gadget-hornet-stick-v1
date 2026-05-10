# Module Lifecycle - Gadget V1 (ID) ♻️

Dokumen ini menjelaskan alur hidup (lifecycle) setiap modul atau fitur di Gadget V1.

## 🔄 Pola Lifecycle Standar

Setiap fitur yang memiliki state sendiri harus mengikuti pola 4 fase:

### 1. Inisialisasi (`onEnter` / Triggered)
Dilakukan saat transisi state di `main.cpp`.
- **Tugas**: Alokasi resource (misal: panggil `rfManager.start()`), reset counter, dan menggambar UI awal.
- **Catatan**: Fase ini hanya berjalan **sekali** saat masuk ke fitur.

### 2. Pemrosesan (`update()`)
Dipanggil setiap kali loop berjalan selama state tersebut aktif.
- **Tugas**: Membaca sensor, memproses paket WiFi, menangani navigasi tombol khusus.
- **Catatan**: Harus bersifat **non-blocking** (tidak boleh ada loop lama atau `delay`).

### 3. Visualisasi (`render()`)
Biasanya bagian dari `update` atau loop timer.
- **Tugas**: Mengirim data ke buffer OLED.
- **Catatan**: Gunakan interval waktu (misal tiap 500ms) agar CPU tidak terbebani rendering terus menerus.

### 4. Pembersihan (`onExit` / Cleanup)
Dilakukan saat user menekan tombol "Back" (Left).
- **Tugas**: Mematikan radio WiFi, melepaskan sensor, menyimpan konfigurasi ke LittleFS.
- **Catatan**: **MANDATORY**. Kegagalan melakukan cleanup akan menyebabkan crash atau kegagalan modul lain (terutama modul WiFi).

---

## 📋 Contoh Kasus

### WiFi Repeater Lifecycle
1. **Enter**: Memuat config dari Flash, inisialisasi mode AP+STA.
2. **Update**: Menjalankan background NAT routing dan DNS proxy.
3. **Exit**: Panggil `stopRepeater()`, mematikan radio total, kembali ke menu utama.

### RF Analyzer Lifecycle
1. **Enter**: Panggil `sniffer_start()`, aktifkan *Promiscuous Mode*.
2. **Update**: Menghitung paket per detik (PPS) dan melakukan *Channel Hopping*.
3. **Exit**: Panggil `sniffer_stop()`, kembalikan WiFi ke mode OFF.
