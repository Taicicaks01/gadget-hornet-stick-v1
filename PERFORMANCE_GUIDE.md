# Performance Guide - Gadget V1 (ID) 🚀

Strategi untuk menjaga performa maksimal pada hardware ESP8266 yang terbatas.

## 🧠 RAM Optimization (80KB)
- **F() Macro**: Gunakan `F("String")` untuk setiap teks statis agar disimpan di Flash (4MB), bukan RAM.
- **Heap Monitoring**: Panggil `ESP.getFreeHeap()` selama pengembangan. Jika angka ini terus menurun, periksa kebocoran memori (Memory Leak).
- **Static Objects**: Gunakan objek global atau singleton daripada membuat objek lokal di dalam fungsi yang sering dipanggil.

## ⚡ CPU & Loop Timing
- **Millis Timing**: Gunakan pola ini daripada `delay()`:
  ```cpp
  if (millis() - lastUpdate > 1000) {
      lastUpdate = millis();
      // Logic here
  }
  ```
- **Yielding**: Panggil `yield()` atau `delay(0)` jika melakukan loop intensif (misal: memproses data LittleFS yang besar) agar sistem WiFi background tidak terputus dan memicu WDT reset.

## 📺 OLED Performance
- **Partial Refresh**: Hanya update area layar yang angkanya berubah.
- **I2C Speed**: Pastikan `Wire.setClock(400000)` dipanggil untuk memaksimalkan bandwidth I2C.
- **Minimize display.display()**: Mengirim buffer 1KB ke layar memakan waktu ~30-40ms. Jangan panggil lebih dari 20 kali per detik.
- **Fullscreen Animation Strategy**: Untuk running text fullscreen, lakukan clear sekali saat masuk lalu redraw band animasi secara parsial; ini mengurangi artifact, overlap, dan framebuffer corruption.
- **Refresh Cap**: Batasi animasi fullscreen ke 10–20 FPS (misal 66–100ms per frame) untuk menjaga smoothness dan menghemat CPU.
- **No Overlay Noise**: Jangan menggambar header/footer/status bar ketika mode fullscreen aktif; hindari redraw UI lain saat animasi berjalan.

## 📡 WiFi Efficiency
- **Power Save**: Matikan radio (`WiFi.mode(WIFI_OFF)`) saat sedang tidak menggunakan fitur radio (misal: saat di menu Dashboard atau Ruler) untuk menghemat baterai dan mengurangi panas chip.
- **Background Tasks**: Sadari bahwa NAPT dan DNS Proxy berjalan di background. Jangan membebani CPU dengan kalkulasi berat saat mode Repeater aktif.

## 📂 LittleFS Efficiency
- **Read-Heavy**: LittleFS dioptimalkan untuk pembacaan. Hindari penulisan (writing) yang terlalu sering ke flash karena akan memperpendek umur hardware dan menyebabkan lag UI.
- **Batching**: Jika harus menulis banyak data (misal: RF Logger), kumpulkan di buffer RAM dulu sebelum menulis satu blok besar ke file.
