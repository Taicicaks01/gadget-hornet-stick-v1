# Feature Documentation - Gadget V1 📂

Dokumen ini menjelaskan detail teknis, alur logika, dan implementasi setiap fitur di Gadget V1.

---

## 1. ENV Dashboard 🌡️
Dashboard lingkungan yang menyatukan data dari sensor I2C.
- **Hardware**: BME280.
- **Cara Kerja**: Membaca register sensor setiap 2 detik. Menggunakan formula tekanan permukaan laut standar untuk estimasi ketinggian (*Altitude*).
- **UI Behavior**: Menampilkan 4 parameter utama (Temp, Hum, Pres, Alt) dengan progress bar baterai di bagian atas.
- **Optimization**: Menggunakan pembacaan sensor non-blocking agar UI tetap responsif.

## 2. Smart Ruler 📏
Alat ukur jarak presisi tinggi.
- **Hardware**: VL53L0X (Laser Time-of-Flight).
- **Cara Kerja**: Mengirimkan pulsa foton dan menghitung waktu pantulan. Akurasi mencapai +/- 3% dalam rentang 2 meter.
- **UI Behavior**: Menampilkan angka jarak dalam satuan *mm* atau *cm* secara dinamis. Menampilkan pesan "OUT OF RANGE" jika pantulan laser tidak terdeteksi.
- **Lifecycle**: Sensor diinisialisasi hanya saat masuk ke menu Ruler untuk menghemat daya.

## 3. Universal IR Tools 📺
Modul kontrol peralatan elektronik.
- **Hardware**: IR Receiver (D5), IR Transmitter (D8).
- **Fitur**:
  - **IR Clone**: Menangkap sinyal remote apa saja dan menyimpannya di RAM untuk dikirim ulang.
  - **IR Manage**: Navigasi ke file `.ir` di LittleFS untuk dikirim.
- **File Structure**: Sinyal IR disimpan dalam format HEX di direktori `/ir/`.

## 4. 2.4GHz Analyzer 📉
Alat analisis spektrum WiFi pasif.
- **Mode**:
  - **Live RF**: Grafik paket per detik (PPS) real-time.
  - **Spike Detector**: Mendeteksi lonjakan trafik yang mencurigakan di kanal tertentu.
  - **Channel Heatmap**: Visualisasi kanal 1-13 untuk melihat kanal mana yang paling padat.
- **Limitation**: ESP8266 hanya memiliki satu radio. Mode ini menggunakan *Promiscuous Mode* yang mematikan koneksi WiFi normal untuk mendengarkan paket mentah di udara.
- **Logic**: Menggunakan *Channel Hopping* (berpindah kanal setiap 100-300ms) untuk mendapatkan gambaran spektrum yang utuh.

## 5. WiFi Repeater 🔁
Router mini portabel.
- **Hardware**: Internal WiFi ESP8266.
- **Cara Kerja**: Mengaktifkan mode AP+STA secara bersamaan. Menggunakan modul LwIP NAPT untuk meneruskan paket dari klien AP ke upstream router.
- **Transfer Monitor**: Taksiran trafik (Heuristic) berdasarkan hit DNS dan jumlah klien.
- **Config Storage**: Profil jaringan disimpan di `/config/net_X.cfg`. Pengaturan AP (SSID/Pass) disimpan di `/config/ap_config.cfg`.
- **Security**: Mendukung mode WPA2-PSK dan OPEN (tanpa password).

---

### Integrasi LittleFS
Sistem menggunakan LittleFS untuk persistensi data:
- **Konfigurasi**: Disimpan dalam format teks sederhana agar ringan saat dibaca.
- **Logging**: Digunakan oleh RF Analyzer untuk mencatat event penting di `/rf/`.

### Batasan ESP8266
- **Single Radio**: Anda tidak bisa menjalankan WiFi Repeater dan RF Analyzer secara bersamaan. Salah satu harus mengalah dan mematikan radio sebelum yang lain bisa mulai.
- **CPU Speed**: Rendering OLED dilakukan secara parsial untuk mencegah stuttering saat trafik jaringan tinggi.
