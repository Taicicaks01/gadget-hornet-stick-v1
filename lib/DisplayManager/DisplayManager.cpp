#include "DisplayManager.h"
#include "config.h"
#include "assets.h"
#include <LittleFS.h>
#include <string.h>

DisplayManager displayManager;

DisplayManager::DisplayManager() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET) {}

bool DisplayManager::init() {
    if(!display.begin(SSD1306_SWITCHCAPVCC, I2C_OLED)) {
        return false;
    }
    display.setTextColor(SSD1306_WHITE);
    display.setTextWrap(false);
    return true;
}

void DisplayManager::clear() {
    display.clearDisplay();
}

void DisplayManager::showMessage(const String& msg, int x, int y, int size) {
    display.clearDisplay();
    display.setTextSize(size);
    display.setCursor(x, y);
    display.println(msg);
    display.display();
}

void DisplayManager::showSending() {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.println("SENDING...");
    display.display();
}

void DisplayManager::drawDashboard(float temp, float hum, float pres, float alt, int bat_percent) {
    display.clearDisplay();

    // --- HEADER (Inverted Rectangle) ---
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setTextSize(1);
    display.setCursor(2, 2);
    display.print("ENV MONITOR");

    // Battery Icon & Text
    display.setCursor(95, 2);
    display.print(bat_percent);
    display.print("%");
    
    // Battery Box
    display.drawRect(114, 2, 12, 7, SSD1306_BLACK);
    display.fillRect(126, 4, 2, 3, SSD1306_BLACK);
    // Battery Fill
    int batFill = (bat_percent * 10) / 100;
    if (batFill > 0) {
        display.fillRect(115, 3, batFill, 5, SSD1306_BLACK);
    }

    // Kembalikan text color ke putih untuk konten
    display.setTextColor(SSD1306_WHITE);

    // --- BARIS 1: SUHU & KELEMBAPAN ---
    // Suhu
    display.drawBitmap(2, 16, icon_temp, 8, 8, SSD1306_WHITE);
    display.setCursor(14, 16);
    display.print(temp, 1);
    display.print(" C");

    // Kelembapan
    display.drawBitmap(70, 16, icon_hum, 8, 8, SSD1306_WHITE);
    display.setCursor(82, 16);
    display.print(hum, 0);
    display.print(" %");

    // --- GARIS PEMISAH 1 ---
    display.drawLine(0, 28, SCREEN_WIDTH, 28, SSD1306_WHITE);

    // --- BARIS 2: TEKANAN ---
    display.drawBitmap(2, 34, icon_pres, 8, 8, SSD1306_WHITE);
    display.setCursor(14, 34);
    display.print(pres, 1);
    display.print(" hPa");

    // --- GARIS PEMISAH 2 ---
    display.drawLine(0, 46, SCREEN_WIDTH, 46, SSD1306_WHITE);

    // --- BARIS 3: KETINGGIAN & SYSTEM INFO ---
    // Ketinggian (Kiri)
    display.drawBitmap(2, 52, icon_alt, 8, 8, SSD1306_WHITE);
    display.setCursor(14, 52);
    display.print(alt, 1);
    display.print(" m");

    // System Info: Memo/Storage (Kanan)
    display.drawBitmap(78, 52, icon_memo, 8, 8, SSD1306_WHITE);
    display.setCursor(88, 52);
    
    FSInfo fs_info;
    LittleFS.info(fs_info);
    int memPercent = 0;
    if(fs_info.totalBytes > 0) {
        memPercent = (fs_info.usedBytes * 100) / fs_info.totalBytes;
    }
    display.print(memPercent);
    display.print("%");

    display.display();
}

void DisplayManager::showBootAnimation() {
    // Menampilkan logo Hornet dengan warna yang di-invert 
    // (background hitam, logo putih)
    display.clearDisplay();
    display.drawBitmap(0, 0, hornet_frame_main, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_BLACK, SSD1306_WHITE);
    display.display();
    
    // Tahan logo selama 2 detik
    delay(2000);

    display.clearDisplay();
    display.display();
}

void DisplayManager::drawMainMenu(int selectedIndex) {
    display.clearDisplay();
    
    // --- HEADER ---
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setTextSize(1);
    display.setCursor(38, 2);
    display.print("MAIN MENU");

    // --- DAFTAR MENU ---
    display.setTextSize(1);
    const char* items[] = {"1. ENV Dashboard", "2. Smart Ruler", "3. IR Tools", "4. 2.4GHz Analyzer", "5. WiFi Repeater", "6. Onscreen Display", "7. Settings"};
    
    // Calculate display window (show 4 items max)
    int startIdx = selectedIndex - (selectedIndex % 4);

    for(int i = 0; i < 4 && (startIdx + i) < 7; i++) {
        int idx = startIdx + i;
        int y = 18 + (i * 12);
        if(idx == selectedIndex) {
            display.fillRect(0, y - 2, SCREEN_WIDTH, 11, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);
        } else {
            display.setTextColor(SSD1306_WHITE);
        }
        display.setCursor(10, y);
        display.print(items[idx]);
    }
    display.display();
}

void DisplayManager::drawOSDMenu(int selectedIndex) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setTextSize(1);
    display.setCursor(18, 2);
    display.print(F("ONSCREEN DISPLAY"));

    const char* items[] = {"1. Custom Text", "2. Running Text", "3. Eye Animation"};
    for(int i = 0; i < 3; i++) {
        int y = 18 + (i * 14);
        if(i == selectedIndex) {
            display.fillRect(0, y - 2, SCREEN_WIDTH, 11, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);
        } else {
            display.setTextColor(SSD1306_WHITE);
        }
        display.setCursor(10, y);
        display.print(items[i]);
    }
    display.display();
}

void DisplayManager::drawRuler(int range, bool isValid) {
    display.clearDisplay();

    // --- HEADER ---
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setTextSize(1);
    display.setCursor(30, 2);
    display.print("SMART RULER");

    // --- ANGKA UTAMA ---
    display.setTextColor(SSD1306_WHITE);
    if(isValid) {
        display.setTextSize(3);
        
        // Tengah
        String rangeStr = String(range);
        int textWidth = rangeStr.length() * 18; // Approx 18px per char in Size 3
        int xPos = (SCREEN_WIDTH - textWidth) / 2 - 10;
        if(xPos < 0) xPos = 0;
        
        display.setCursor(xPos, 22);
        display.print(rangeStr);
        
        display.setTextSize(1);
        display.print("mm");
    } else {
        display.setTextSize(1);
        display.setCursor(25, 28);
        display.print("Out Of Range");
    }

    // --- INSTRUKSI TOMBOL ---
    display.setTextSize(1);
    display.setCursor(0, 54);
    display.print("< Back     [OK] Zero");

    display.display();
}
void DisplayManager::drawIRMenu(int selectedIndex) {
    display.clearDisplay();
    
    // --- HEADER ---
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setTextSize(1);
    display.setCursor(35, 2);
    display.print("IR CLONER");

    // --- DAFTAR MENU ---
    display.setTextSize(1);
    const char* items[] = {"1. Clone Signal", "2. Bruteforce", "3. Manage Files"};
    
    for(int i = 0; i < 3; i++) {
        int y = 18 + (i * 14);
        if(i == selectedIndex) {
            display.fillRect(0, y - 2, SCREEN_WIDTH, 11, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);
        } else {
            display.setTextColor(SSD1306_WHITE);
        }
        display.setCursor(10, y);
        display.print(items[i]);
    }
    display.display();
}

void DisplayManager::drawIRCloneWaiting(const String& lastCode) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(30, 2);
    display.print("CLONE MODE");

    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 20);
    display.print("Waiting for IR...");
    
    display.setCursor(0, 35);
    display.print("Last: ");
    display.print(lastCode);

    display.setCursor(0, 54);
    display.print("< Back     [OK] Save");
    display.display();
}

void DisplayManager::drawBruteforceProgress(int current, int total, const String& codeName) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(30, 2);
    display.print("BRUTEFORCE");

    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 20);
    display.print("Sending: ");
    display.print(current);
    display.print(" / ");
    display.print(total);

    display.setCursor(0, 35);
    display.print("Code: ");
    display.print(codeName);

    display.setCursor(0, 54);
    display.print("< Stop       [OK] Got It");
    display.display();
}

void DisplayManager::drawFileManager(const String& filename, int fileIndex, int totalFiles) {
    display.clearDisplay();
    display.setTextSize(1); // Mencegah bug ukuran font dari showSending()
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(25, 2);
    display.print("FILE MANAGER");

    display.setTextColor(SSD1306_WHITE);
    
    if(totalFiles == 0) {
        display.setCursor(20, 30);
        display.print("No Files Found");
    } else {
        display.setCursor(0, 20);
        display.print("File ");
        display.print(fileIndex + 1);
        display.print(" of ");
        display.print(totalFiles);

        // Ubah ukuran font menjadi 1 agar tidak tertindih untuk nama file yang panjang
        display.setTextSize(1);
        display.setCursor(0, 35);
        display.print("Name: ");
        if(filename == "") display.print("Empty");
        else display.print(filename);
    }

    display.setCursor(0, 54);
    display.print("< Back [OK]Send [^]Del");
    display.display();
}

void DisplayManager::drawOSDCustomTextEditor(const char* text, int kx, int ky) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setTextSize(1);
    display.setCursor(20, 2);
    display.print(F("CUSTOM TEXT"));

    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 14);
    display.print(text);

    static const char keys[3][13] = {
        "ABCDEFGHIJKL",
        "MNOPQRSTUVWX",
        "YZ0123456789"
    };

    for (int row = 0; row < 3; row++) {
        int y = 34 + (row * 9);
        for (int col = 0; col < 12; col++) {
            int x = 2 + (col * 10);
            if (col == kx && row == ky) {
                display.fillRect(x - 1, y - 1, 9, 8, SSD1306_WHITE);
                display.setTextColor(SSD1306_BLACK);
            } else {
                display.setTextColor(SSD1306_WHITE);
            }
            display.setCursor(x, y);
            display.write(keys[row][col]);
        }
    }

    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 58);
    display.print(F("LEFT del  OK add  hold save"));
    display.display();
}

void DisplayManager::drawOSDRunningText(const char* text, int scrollOffset, uint16_t speedMs, bool repeatMode, uint8_t spacingPx, bool paused, bool centeredMode) {
    (void)repeatMode;
    (void)speedMs;
    (void)centeredMode;

    // Fullscreen animation band: clear only the active band to avoid artifacts.
    display.fillRect(0, 16, SCREEN_WIDTH, 32, SSD1306_BLACK);

    const uint8_t textSize = 2;
    const int charWidth = 6 * textSize;
    const int textWidth = static_cast<int>(strlen(text)) * charWidth;
    const int textHeight = 8 * textSize;
    const int baselineY = (SCREEN_HEIGHT - textHeight) / 2 + 2;

    display.setTextSize(textSize);
    display.setTextColor(paused ? SSD1306_WHITE : SSD1306_WHITE);

    int drawX = scrollOffset;
    if (drawX < -textWidth - spacingPx) {
        drawX = SCREEN_WIDTH;
    }

    display.setCursor(drawX, baselineY);
    display.print(text);

    // Draw a second copy only when the text needs spacing continuity.
    if (textWidth < SCREEN_WIDTH || repeatMode) {
        int nextX = drawX + textWidth + spacingPx;
        if (nextX < SCREEN_WIDTH) {
            display.setCursor(nextX, baselineY);
            display.print(text);
        }
    }

    display.display();
}

void DisplayManager::drawOSDEyeAnimationStub(uint8_t blinkPhase) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setTextSize(1);
    display.setCursor(20, 2);
    display.print(F("EYE ANIMATION"));

    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor(18, 20);
    display.print(F("COMING"));
    display.setCursor(26, 40);
    display.print(F("SOON"));

    if (blinkPhase) {
        display.fillCircle(106, 32, 8, SSD1306_WHITE);
        display.fillCircle(106, 32, 3, SSD1306_BLACK);
    } else {
        display.drawCircle(106, 32, 8, SSD1306_WHITE);
        display.drawCircle(106, 32, 3, SSD1306_WHITE);
    }
    display.display();
}

void DisplayManager::drawSettingsMenu(int selectedIndex) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setTextSize(1);
    display.setCursor(34, 2);
    display.print(F("SETTINGS"));

    const char* items[] = {"1. Screen Sleep"};
    for(int i = 0; i < 1; i++) {
        int y = 24;
        if(i == selectedIndex) {
            display.fillRect(0, y - 2, SCREEN_WIDTH, 11, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);
        } else {
            display.setTextColor(SSD1306_WHITE);
        }
        display.setCursor(10, y);
        display.print(items[i]);
    }
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 54);
    display.print(F("LEFT back   OK open"));
    display.display();
}

void DisplayManager::drawScreenSleepMenu(bool enabled, uint32_t timeoutMs) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setTextSize(1);
    display.setCursor(28, 2);
    display.print(F("SCREEN SLEEP"));

    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 18);
    display.print(F("STATUS:"));
    display.setCursor(48, 18);
    display.print(enabled ? F("ON") : F("OFF"));

    display.setCursor(0, 32);
    display.print(F("TIMEOUT:"));
    display.setCursor(54, 32);
    display.print(timeoutMs / 1000);
    display.print(F(" SEC"));

    display.setCursor(0, 46);
    display.print(F("UP/DN adjust  OK toggle"));
    display.setCursor(0, 56);
    display.print(F("LEFT back"));
    display.display();
}

void DisplayManager::setDisplaySleep(bool enabled) {
    if (enabled) {
        display.ssd1306_command(SSD1306_DISPLAYOFF);
    } else {
        display.ssd1306_command(SSD1306_DISPLAYON);
    }
}

Adafruit_SSD1306* DisplayManager::getDisplay() {
    return &display;
}
void DisplayManager::drawRFMenu(int selectedIndex) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setTextSize(1);
    display.setCursor(20, 2);
    display.print("2.4GHz ANALYZER");

    display.setTextColor(SSD1306_WHITE);
    const char* items[] = {"1. Live RF", "2. Deauth Monitor", "3. Beacon Flood", "4. Spike Detector", "5. Packet Stats", "6. Channel Heatmap", "7. RF Radar", "8. RF Logger", "9. Log Viewer"};
    
    // Calculate display window (show 4 items max)
    int startIdx = selectedIndex - (selectedIndex % 4);
    
    for(int i = 0; i < 4 && (startIdx + i) < 9; i++) {
        int idx = startIdx + i;
        int y = 18 + (i * 11);
        if(idx == selectedIndex) {
            display.fillRect(0, y - 1, SCREEN_WIDTH, 10, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);
        } else {
            display.setTextColor(SSD1306_WHITE);
        }
        display.setCursor(2, y);
        display.print(items[idx]);
    }
    display.display();
}

void DisplayManager::drawRFLive(int rssi, int pps, const String& status) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(30, 2);
    display.print("LIVE RF SNIFF");

    display.setTextColor(SSD1306_WHITE);
    
    // Segmented Signal Bar
    int barWidth = map(constrain(rssi, -100, -30), -100, -30, 0, 10); // 10 segments
    display.setCursor(2, 16); display.print("SIGNAL:");
    
    for(int i=0; i<10; i++) {
        if(i < barWidth) display.fillRect(55 + (i*7), 16, 5, 8, SSD1306_WHITE);
        else display.drawRect(55 + (i*7), 16, 5, 8, SSD1306_WHITE);
    }

    // Two column layout
    display.drawFastHLine(0, 28, 128, SSD1306_WHITE);
    
    display.setTextSize(2);
    display.setCursor(2, 33); display.print(rssi); 
    display.setTextSize(1);
    display.print("dBm");

    display.setTextSize(2);
    display.setCursor(75, 33); display.print(pps);
    display.setTextSize(1);
    display.setCursor(75, 52); display.print("PPS");

    display.drawFastHLine(0, 52, 65, SSD1306_WHITE);
    display.setCursor(2, 55); 
    display.print("ST: "); display.print(status);
    
    display.display();
}

void DisplayManager::drawRFDeauth(int mgmt, const String& status) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(20, 2);
    display.print("DEAUTH DETECTOR");

    display.setTextColor(SSD1306_WHITE);
    display.drawRect(0, 15, 128, 49, SSD1306_WHITE);
    
    display.setCursor(5, 20); display.print("MGMT PACKETS/s");
    display.drawFastHLine(5, 30, 118, SSD1306_WHITE);
    
    display.setTextSize(2);
    if(mgmt > 100) {
        display.fillRect(5, 35, 118, 25, SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK);
        display.setCursor(15, 40); display.print("WARNING!");
    } else {
        display.setCursor(15, 40); display.print(mgmt);
        display.setTextSize(1);
        display.setCursor(65, 47); display.print("SAFE");
    }

    display.display();
}

void DisplayManager::drawRFBeacon(int ssidCount, const String& status) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(25, 2);
    display.print("BEACON FLOOD");

    display.setTextColor(SSD1306_WHITE);
    
    // Draw a fake router icon
    display.drawRect(5, 25, 20, 15, SSD1306_WHITE);
    display.drawLine(10, 25, 5, 15, SSD1306_WHITE);
    display.drawLine(20, 25, 25, 15, SSD1306_WHITE);

    display.setCursor(35, 18); display.print("DETECTED SSIDs:");
    
    display.setTextSize(2);
    display.setCursor(35, 32); display.print(ssidCount);
    
    display.setTextSize(1);
    display.drawFastHLine(35, 50, 93, SSD1306_WHITE);
    display.setCursor(35, 54); display.print(status);

    display.display();
}

void DisplayManager::drawRFSpike(const String& level, int peakRssi) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(20, 2);
    display.print("SPIKE DETECTOR");

    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(5, 18); display.print("PEAK HOLD:");
    display.setTextSize(2);
    display.setCursor(5, 30); display.print(peakRssi); 
    display.setTextSize(1); display.print(" dB");
    
    display.drawFastHLine(0, 48, 128, SSD1306_WHITE);
    display.setCursor(5, 52); display.print("LVL: "); display.print(level);

    // Mini graph visual
    display.drawLine(80, 45, 90, 45, SSD1306_WHITE);
    display.drawLine(90, 45, 100, 20, SSD1306_WHITE); // Spike!
    display.drawLine(100, 20, 110, 45, SSD1306_WHITE);
    display.drawLine(110, 45, 125, 45, SSD1306_WHITE);

    display.display();
}

void DisplayManager::drawRFStats(int mgmt, int data, int ctrl, int pps) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(25, 2);
    display.print("PACKET STATS");

    display.setTextColor(SSD1306_WHITE);
    
    // Grid layout
    display.drawRect(0, 15, 64, 24, SSD1306_WHITE);
    display.drawRect(64, 15, 64, 24, SSD1306_WHITE);
    display.drawRect(0, 39, 64, 24, SSD1306_WHITE);
    display.drawRect(64, 39, 64, 24, SSD1306_WHITE);

    display.setCursor(4, 19); display.print("MGMT"); display.setCursor(4, 29); display.print(mgmt);
    display.setCursor(68, 19); display.print("DATA"); display.setCursor(68, 29); display.print(data);
    display.setCursor(4, 43); display.print("CTRL"); display.setCursor(4, 53); display.print(ctrl);
    
    display.setCursor(68, 43); display.print("PPS"); 
    display.fillRect(65, 50, 62, 12, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(68, 52); display.print(pps);
    
    display.display();
}

void DisplayManager::drawRFHeatmap(int* channels) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(20, 2);
    display.print("CH SPECTRUM");

    display.setTextColor(SSD1306_WHITE);
    
    // Draw grid lines
    for(int y=15; y<=55; y+=10) {
        for(int x=0; x<128; x+=4) display.drawPixel(x, y, SSD1306_WHITE);
    }
    
    // Draw 13 bars for CH1 to CH13
    for(int i = 0; i < 13; i++) {
        int x = i * 9 + 6;
        int rssi = channels[i];
        int h = 0;
        if(rssi > -100) {
            h = map(constrain(rssi, -100, -30), -100, -30, 1, 40);
        }
        // Draw Bar
        display.fillRect(x, 55 - h, 6, h, SSD1306_WHITE);
        
        // CH Labels
        if(i==0 || i==5 || i==10) { 
            display.setCursor(x, 57);
            display.print(i+1);
        }
    }
    display.display();
}

void DisplayManager::drawRFRadar(int rssi, const String& level) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(35, 2);
    display.print("RF RADAR");

    display.setTextColor(SSD1306_WHITE);
    
    // Radar center
    int cx = 35;
    int cy = 38;
    
    int radius = map(constrain(rssi, -100, -30), -100, -30, 0, 24);
    
    // Crosshair
    display.drawLine(cx, cy-25, cx, cy+25, SSD1306_WHITE);
    display.drawLine(cx-25, cy, cx+25, cy, SSD1306_WHITE);
    
    display.drawCircle(cx, cy, 24, SSD1306_WHITE);
    display.drawCircle(cx, cy, 16, SSD1306_WHITE);
    display.drawCircle(cx, cy, 8, SSD1306_WHITE);
    
    // Fill based on signal
    if(radius > 0) {
        display.fillCircle(cx, cy, radius, SSD1306_WHITE);
    }

    // Data Box
    display.drawRect(70, 15, 58, 48, SSD1306_WHITE);
    display.setCursor(74, 20); display.print("PWR:");
    display.setCursor(74, 30); display.print(rssi); 
    display.setCursor(74, 45); display.print(level);

    display.display();
}

void DisplayManager::drawRFLogger(const String& autoLog, int interval, int files) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(35, 2);
    display.print("RF LOGGER");

    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 20); display.print("AUTO LOG : "); display.print(autoLog);
    display.setCursor(0, 35); display.print("INTERVAL : "); display.print(interval); display.print(" SEC");
    display.setCursor(0, 50); display.print("FILES    : "); display.print(files);

    display.display();
}

void DisplayManager::drawRFLogViewer(const String& filename, const String& line1, const String& line2, const String& line3) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(20, 2);
    display.print("RF LOG VIEWER");

    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 15); display.print(">"); display.print(filename);
    display.setCursor(0, 30); display.print(line1);
    display.setCursor(0, 42); display.print(line2);
    display.setCursor(0, 54); display.print(line3);

    display.display();
}

// ==========================================
// WIFI REPEATER UI
// ==========================================

void DisplayManager::drawRepeaterMenu(int selectedIndex, bool hasConfig) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(25, 2);
    display.print("WIFI REPEATER");

    display.setTextColor(SSD1306_WHITE);
    
    const char* items[] = {"1. Start Repeater", "2. Scan Jaringan", "3. Saved Networks", "4. Repeater Config"};
    
    for(int i = 0; i < 4; i++) {
        int y = 14 + (i * 10);
        if(i == selectedIndex) {
            display.fillRect(0, y - 1, SCREEN_WIDTH, 9, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);
        } else {
            display.setTextColor(SSD1306_WHITE);
        }
        display.setCursor(2, y);
        display.print(items[i]);
    }

    if(hasConfig) {
        display.setTextColor(SSD1306_WHITE);
        display.drawFastHLine(0, 54, 128, SSD1306_WHITE);
        display.setCursor(2, 56);
        display.print("Config: TERSIMPAN");
    } else {
        display.setTextColor(SSD1306_WHITE);
        display.drawFastHLine(0, 54, 128, SSD1306_WHITE);
        display.setCursor(2, 56);
        display.print("Config: KOSONG");
    }

    display.display();
}

void DisplayManager::drawWiFiList(int selectedIndex, int totalSSID, const String& ssidName, int rssi, bool isSecure) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(25, 2);
    display.print("PILIH JARINGAN");

    display.setTextColor(SSD1306_WHITE);
    
    if (totalSSID == 0) {
        display.setCursor(10, 30);
        display.print("Mencari WiFi...");
        display.display();
        return;
    }

    // Scroll bar calculation
    int barHeight = 40 / totalSSID;
    if (barHeight < 5) barHeight = 5;
    int barY = 15 + (selectedIndex * (40 - barHeight) / (totalSSID > 1 ? totalSSID - 1 : 1));
    display.drawRect(124, 15, 4, 40, SSD1306_WHITE);
    display.fillRect(124, barY, 4, barHeight, SSD1306_WHITE);

    display.setCursor(0, 15);
    display.print("IDX: "); display.print(selectedIndex + 1); display.print("/"); display.print(totalSSID);
    
    display.setCursor(0, 30);
    String truncSSID = ssidName;
    if (truncSSID.length() > 20) truncSSID = truncSSID.substring(0, 19) + ".";
    display.print(truncSSID);

    display.setCursor(0, 45);
    if(isSecure) display.print("[LOCK] "); else display.print("[OPEN] ");
    display.print(rssi); display.print(" dBm");

    display.display();
}

void DisplayManager::drawVirtualKeyboard(const String& currentText, int kx, int ky) {
    display.clearDisplay();
    
    // Top Bar (Current Text)
    display.drawRect(0, 0, 128, 14, SSD1306_WHITE);
    display.setCursor(2, 3);
    
    // Tampilkan 20 huruf terakhir jika kepanjangan
    String showText = currentText;
    if(showText.length() > 20) {
        showText = ">" + showText.substring(showText.length() - 19);
    }
    display.print(showText);
    
    // Blinking cursor
    if((millis() / 500) % 2 == 0) {
        display.print("_");
    }

    // Grid Keyboard (A-Z, 0-9)
    // 3 baris x 12 kolom = 36 char
    const char keys[3][13] = {
        "ABCDEFGHIJKL",
        "MNOPQRSTUVWX",
        "YZ0123456789"
    };

    for(int y=0; y<3; y++) {
        for(int x=0; x<12; x++) {
            int px = 2 + (x * 10);
            int py = 20 + (y * 12);
            
            if(kx == x && ky == y) {
                display.fillRect(px-1, py-1, 9, 11, SSD1306_WHITE);
                display.setTextColor(SSD1306_BLACK);
            } else {
                display.setTextColor(SSD1306_WHITE);
            }
            display.setCursor(px, py);
            display.print(keys[y][x]);
        }
    }
    
    display.setTextColor(SSD1306_WHITE);
    display.drawFastHLine(0, 56, 128, SSD1306_WHITE);
    display.setCursor(0, 57);
    display.print("L:Del  Hold OK:Enter");

    display.display();
}

void DisplayManager::drawRepeaterConnecting(int secondsLeft, const String& ssid) {
    display.clearDisplay();
    display.drawRect(10, 10, 108, 44, SSD1306_WHITE);
    
    display.setCursor(25, 15);
    display.print("CONNECTING...");
    
    display.setCursor(15, 30);
    String tr = ssid;
    if(tr.length() > 15) tr = tr.substring(0,14) + ".";
    display.print(tr);

    display.setCursor(25, 45);
    display.print("Timeout: "); display.print(secondsLeft); display.print("s");
    
    display.display();
}

void DisplayManager::drawRepeaterStatus(const String& ssid, const String& ip, int clients, int rssi) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(15, 2);
    display.print("HORNET REPEATER");

    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(0, 16); display.print("UP: "); 
    String up = ssid; if(up.length() > 16) up = up.substring(0, 15);
    display.print(up);
    
    display.setCursor(0, 28); display.print("IP: "); display.print(ip);
    
    display.setCursor(0, 40); display.print("CLI: "); display.print(clients);
    display.setCursor(64, 40); display.print("SIG: "); display.print(rssi);
    
    display.drawFastHLine(0, 54, 128, SSD1306_WHITE);
    display.setCursor(0, 56); display.print("AP: HORNET-REPEATER");

    display.display();
}

void DisplayManager::drawRepeaterMonitor(uint64_t rx, uint64_t tx, uint32_t upSpd, uint32_t downSpd, int clients) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(18, 2);
    display.print("TRANSFER MONITOR");

    display.setTextColor(SSD1306_WHITE);
    
    // Formatting speed KB/s or MB/s
    auto formatSpeed = [](uint32_t bytesPerSec) -> String {
        if(bytesPerSec > 1024*1024) return String(bytesPerSec / (1024.0*1024.0), 1) + " MB/s";
        return String(bytesPerSec / 1024.0, 1) + " KB/s";
    };

    auto formatBytes = [](uint64_t bytes) -> String {
        if(bytes > 1024*1024*1024) return String((uint32_t)(bytes / (1024*1024*1024))) + " GB";
        if(bytes > 1024*1024) return String((uint32_t)(bytes / (1024*1024))) + " MB";
        return String((uint32_t)(bytes / 1024)) + " KB";
    };

    display.setCursor(0, 16); display.print("DWN: "); display.print(formatSpeed(downSpd));
    display.setCursor(64, 16); display.print(formatBytes(rx));

    display.setCursor(0, 28); display.print("UP : "); display.print(formatSpeed(upSpd));
    display.setCursor(64, 28); display.print(formatBytes(tx));

    display.setCursor(0, 42); display.print("CLIENTS: "); display.print(clients);
    
    // Burst indicator
    if (upSpd > 1024 || downSpd > 1024) {
        display.fillRect(115, 42, 8, 8, SSD1306_WHITE); // Blinking square
    } else {
        display.drawRect(115, 42, 8, 8, SSD1306_WHITE); // Hollow square
    }

    display.drawFastHLine(0, 54, 128, SSD1306_WHITE);
    display.setCursor(0, 56); display.print("L: Exit");

    display.display();
}

void DisplayManager::drawRepeaterConfigMenu(int selectedIndex, const String& ssid, const String& pass, bool isWpa2, bool isHidden) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(20, 2);
    display.print("REPEATER CONFIG");

    display.setTextColor(SSD1306_WHITE);
    
    String sec = isWpa2 ? "WPA2" : "OPEN";
    String hid = isHidden ? "ON" : "OFF";
    String passCen = pass.length() > 0 ? "********" : "NONE";

    String items[6] = {
        "SSID: " + ssid,
        "Pass: " + passCen,
        "Sec : " + sec,
        "Hide: " + hid,
        "Save & Apply",
        "Factory Reset"
    };

    // Calculate display window (show 4 items max)
    int startIdx = selectedIndex - (selectedIndex % 4);

    for(int i = 0; i < 4 && (startIdx + i) < 6; i++) {
        int idx = startIdx + i;
        int y = 14 + (i * 12);
        if(idx == selectedIndex) {
            display.fillRect(0, y - 1, SCREEN_WIDTH, 10, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);
        } else {
            display.setTextColor(SSD1306_WHITE);
        }
        display.setCursor(2, y);
        String txt = items[idx];
        if(txt.length() > 20) txt = txt.substring(0, 19) + ".";
        display.print(txt);
    }

    display.display();
}

void DisplayManager::drawSavedNetworksMenu(int selectedIndex, int total, const String& ssidName) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, 11, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setCursor(20, 2);
    display.print("SAVED NETWORKS");

    display.setTextColor(SSD1306_WHITE);
    
    if (total == 0) {
        display.setCursor(10, 30);
        display.print("Belum ada data.");
        display.display();
        return;
    }

    display.setCursor(0, 18); display.print("Slot "); display.print(selectedIndex + 1); display.print("/"); display.print(total);
    display.setCursor(0, 32); 
    display.print("> "); 
    String tr = ssidName; if(tr.length() > 18) tr = tr.substring(0, 17) + ".";
    display.print(tr);

    display.drawFastHLine(0, 50, 128, SSD1306_WHITE);
    display.setCursor(0, 54); display.print("OK:Load   R:Delete");

    display.display();
}

void DisplayManager::drawDeleteConfirmMenu(const String& ssidName, bool isYesSelected) {
    display.clearDisplay();
    display.drawRect(5, 5, 118, 54, SSD1306_WHITE);
    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(10, 10); display.print("DELETE NETWORK?");
    
    display.setCursor(10, 25);
    String tr = ssidName; if(tr.length() > 17) tr = tr.substring(0, 16) + ".";
    display.print(tr);

    if(isYesSelected) {
        display.fillRect(15, 40, 40, 12, SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK);
        display.setCursor(20, 42); display.print("[YES]");
        
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(70, 42); display.print("[NO]");
    } else {
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(20, 42); display.print("[YES]");
        
        display.fillRect(65, 40, 35, 12, SSD1306_WHITE);
        display.setTextColor(SSD1306_BLACK);
        display.setCursor(70, 42); display.print("[NO]");
    }

    display.display();
}
