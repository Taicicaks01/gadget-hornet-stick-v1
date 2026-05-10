#ifndef CONFIG_H
#define CONFIG_H

// --- DEFINISI PIN I2C (BME280, VL53L0X, OLED) ---
// Secara default Wemos D1 Mini menggunakan SDA=D2(GPIO4) dan SCL=D1(GPIO5)
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5

// --- DEFINISI LAYAR OLED ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define I2C_OLED 0x3C

// --- DEFINISI ALAMAT SENSOR ---
#define I2C_BME 0x76

// --- DEFINISI PIN IR ---
#define IR_RECEIVE_PIN 14 // D5
#define IR_SEND_PIN 15    // D8

// --- DEFINISI PIN TOMBOL NAVIGASI ---
#define BTN_UP    16 // D0 (Menggunakan eksternal pull-up resistor)
#define BTN_DOWN  12 // D6
#define BTN_OK    13 // D7
#define BTN_LEFT  0  // D3
#define BTN_RIGHT 2  // D4

#endif // CONFIG_H
