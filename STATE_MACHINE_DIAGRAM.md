# State Machine Diagram - Gadget V1 🗺️

Berikut adalah alur transisi kondisi (State) dalam firmware Gadget V1.

---

## 🎨 Diagram Alur Utama

```mermaid
graph TD
    A[STATE_BOOT] -->|Animasi Selesai| B[STATE_NORMAL]
    B -->|Tombol OK/Right| C[STATE_MAIN_MENU]
    C -->|Pilih Ruler| D[STATE_RULER]
    C -->|Pilih IR| E[STATE_IR_MENU]
    C -->|Pilih RF| F[STATE_RF_MENU]
    C -->|Pilih Repeater| G[STATE_REPEATER_MENU]
    
    E --> E1[STATE_IR_CLONE]
    E --> E2[STATE_IR_MANAGE]
    
    F --> F1[STATE_RF_LIVE]
    F --> F2[STATE_RF_HEATMAP]
    F --> F3[STATE_RF_STATS]
    
    G --> G1[STATE_REPEATER_SCAN]
    G --> G2[STATE_REPEATER_CONNECTING]
    G2 -->|Success| G3[STATE_REPEATER_ACTIVE]
    
    D & E1 & E2 & F1 & F2 & F3 & G3 -->|Tombol Left| C
    C -->|Tombol Left| B
```

---

## 📋 Daftar State Detail

| State | Modul | Deskripsi |
| :--- | :--- | :--- |
| **STATE_BOOT** | Display | Menampilkan logo Gadget V1 saat startup. |
| **STATE_NORMAL** | Sensor | Dashboard utama (Suhu, Jarak, dsb). |
| **STATE_MAIN_MENU** | UI | Navigasi vertikal 5 modul utama. |
| **STATE_IR_CLONE** | IR | Menunggu sinyal IR untuk diduplikasi. |
| **STATE_RF_LIVE** | RF | Grafik PPS (Packet Per Second) real-time. |
| **STATE_REPEATER_ACTIVE**| WiFi | Monitoring trafik NAPT yang sedang berjalan. |

*Untuk penjelasan lifecycle, lihat [ARCHITECTURE.md](./ARCHITECTURE.md)*
