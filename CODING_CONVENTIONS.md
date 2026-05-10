# Coding Conventions - Gadget V1 (ID) 📝

Standar penulisan kode untuk menjaga kebersihan dan stabilitas firmware Gadget V1.

## 🏷️ Penamaan (Naming)
- **Class Managers**: Gunakan PascalCase dengan akhiran `Manager`. Contoh: `DisplayManager`, `WiFiRepeaterManager`.
- **Global Objects**: Gunakan camelCase. Contoh: `displayManager`, `rfManager`.
- **Files**: Nama file harus sama dengan nama class. Contoh: `IRManager.h` & `IRManager.cpp`.
- **States**: Gunakan SCREAMING_SNAKE_CASE dengan awalan `STATE_`. Contoh: `STATE_MAIN_MENU`.

---

## 🏗️ Struktur Class Manager
Setiap manager harus memiliki minimal:
- **`begin()`**: Inisialisasi awal hardware.
- **`update()`**: Logika loop utama.
- **`stop()`**: Pembersihan resource sebelum pindah modul.

---

## 🚫 Larangan Keras (Anti-Patterns)
1. **NO delay()**: Jangan pernah gunakan `delay()`. Gunakan timer berbasis `millis()`.
2. **NO String Fragmentation**: Hindari penggunaan operator `+` pada objek `String` di dalam loop utama. Gunakan `F()` macro untuk teks statis.
3. **NO Blocking Loops**: Hindari `while(true)` atau loop panjang yang tidak memanggil `yield()`. Ini akan memicu reset Watchdog Timer (WDT).

---

## 📺 Rendering Style
- Selalu panggil `display.display()` di akhir fungsi render modul.
- Gunakan *Partial Redraw*: Hanya update bagian layar yang berubah untuk menghemat CPU cycle.
- Gunakan `F()` macro pada setiap teks yang dicetak ke OLED: `display.print(F("Text"));`.

---

## 💾 Memory-Safe Patterns
- **Flash Memory**: Simpan tabel data besar atau teks statis di Flash menggunakan `PROGMEM`.
- **Heap Memory**: Hindari alokasi dinamis (`new` / `malloc`) di dalam loop. Alokasikan objek secara statis sebagai member class manager.
