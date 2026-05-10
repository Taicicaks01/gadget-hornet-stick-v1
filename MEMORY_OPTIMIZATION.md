# Memory Optimization - Gadget V1 🧠

ESP8266 memiliki batasan RAM (~80KB) yang cukup ketat. Dokumen ini menjelaskan strategi yang digunakan Gadget V1 untuk tetap stabil dan responsif.

---

## 1. Flash Strings (`F()` Macro) ⚡

Setiap teks statis yang dikirim ke `Serial.print()` atau `display.print()` secara default akan memakan RAM.
- **Strategi**: Gunakan makro `F()` untuk menyimpan string statis di Flash Memory (4MB), bukan di RAM (80KB).
- **Contoh**: `display.print(F("Menu Utama"));`

## 2. String Fragmentation Prevention 🛠️

Penggunaan class `String` secara berlebihan dapat menyebabkan fragmentasi heap, yang berujung pada *crash* acak.
- **Strategi**:
  - Hindari penyambungan string (`+`) di dalam loop utama yang berjalan cepat.
  - Gunakan `reserve()` jika panjang string sudah diketahui.
  - Untuk operasi buffer statis, lebih disarankan menggunakan `char[]` (C-Style strings).

## 3. SSD1306 Buffer Management 📺

Layar OLED 128x64 memakan 1024 bytes (1KB) RAM hanya untuk buffer tampilannya.
- **Strategi**:
  - **Partial Redraw**: Hanya panggil `display.display()` saat ada data yang benar-benar berubah.
  - Hindari menggambar seluruh layar (Clear & Redraw) setiap milidetik. Gunakan `lastDisplayUpdate` timer.

## 4. LittleFS Efficiency 📂

Membaca/menulis file adalah operasi yang lambat dan memakan resource.
- **Strategi**:
  - Gunakan format teks sederhana untuk config agar parsing tidak memakan banyak memori.
  - Pastikan file ditutup (`f.close()`) segera setelah digunakan untuk membebaskan file descriptor.

## 5. WiFi Stack & NAPT RAM 🔁

Fitur WiFi Repeater (NAPT) memakan RAM yang cukup signifikan di level kernel SDK (~15-20KB).
- **Strategi**:
  - Matikan fitur radio yang tidak perlu saat mode Repeater aktif.
  - Gunakan `ip_napt_init(512, 8)` dengan angka yang moderat agar tabel translasi tidak meluap.

## 6. Global Objects Optimization 🧩

Manager classes (`DisplayManager`, `rfManager`, dll) dibuat sebagai objek global statis.
- **Manfaat**: Alokasi memori dilakukan sekali saat boot, sehingga risiko *Out of Memory* saat runtime karena alokasi dinamis dapat diminimalisir.

---

## Ringkasan Penggunaan RAM (v2.0)
| Module | RAM Usage (Est.) | Keterangan |
| :--- | :--- | :--- |
| **System Core** | ~10 KB | Stack, Global Vars, OLED Buffer |
| **WiFi Stack** | ~25 KB | SDK Buffers, LwIP |
| **NAPT Table** | ~10 KB | Hanya saat Repeater aktif |
| **Managers** | ~5 KB | Class instances |
| **FREE RAM** | **~30-40 KB** | Margin aman untuk stabilitas |

*Gunakan `ESP.getFreeHeap()` secara periodik selama pengembangan untuk memantau kebocoran memori.*
