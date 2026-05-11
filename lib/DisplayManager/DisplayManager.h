#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

/**
 * @class DisplayManager
 * @brief Singleton class to manage the SSD1306 OLED display using Adafruit GFX.
 * 
 * This manager handles all visual rendering for the various applications
 * in the Gadget V1 OS. It uses a non-blocking rendering approach.
 */
class DisplayManager {
private:
    Adafruit_SSD1306 display;

public:
    DisplayManager();
    
    /**
     * @brief Initialize the I2C OLED display.
     * @return true if successful, false otherwise.
     */
    bool init();

    /**
     * @brief Clear the display buffer.
     */
    void clear();

    /**
     * @brief Display a simple centered message on the screen.
     */
    void showMessage(const String& msg, int x = 0, int y = 20, int size = 1);

    /**
     * @brief Render the main environment dashboard (Temp, Hum, Pres, Alt).
     */
    void drawDashboard(float temp, float hum, float pres, float alt, int bat_percent);

    /**
     * @brief Render the main scrollable menu.
     */
    void drawMainMenu(int selectedIndex);
    void drawOSDMenu(int selectedIndex);

    /**
     * @brief Render the Smart Ruler UI.
     */
    void drawRuler(int range, bool isValid);

    /**
     * @brief Render the Universal IR menu.
     */
    void drawIRMenu(int selectedIndex);

    void drawIRCloneWaiting(const String& lastCode);
    void drawBruteforceProgress(int current, int total, const String& codeName);
    void drawFileManager(const String& filename, int fileIndex, int totalFiles);

    // --- RF Analyzer Visualization Methods ---
    void drawRFMenu(int selectedIndex);
    void drawRFLive(int rssi, int pps, const String& status);
    void drawRFDeauth(int mgmt, const String& status);
    void drawRFBeacon(int ssidCount, const String& status);
    void drawRFSpike(const String& level, int peakRssi);
    void drawRFStats(int mgmt, int data, int ctrl, int pps);
    void drawRFHeatmap(int* channels);
    void drawRFRadar(int rssi, const String& level);
    void drawRFLogger(const String& autoLog, int interval, int files);
    void drawRFLogViewer(const String& filename, const String& line1, const String& line2, const String& line3);

    // --- WiFi Repeater UI Methods ---
    void drawRepeaterMenu(int selectedIndex, bool hasConfig);
    void drawWiFiList(int selectedIndex, int totalSSID, const String& ssidName, int rssi, bool isSecure);
    void drawVirtualKeyboard(const String& currentText, int kx, int ky);
    void drawRepeaterStatus(const String& ssid, const String& ip, int clients, int rssi);
    void drawRepeaterConnecting(int secondsLeft, const String& ssid);
    void drawRepeaterMonitor(uint64_t rx, uint64_t tx, uint32_t upSpd, uint32_t downSpd, int clients);
    void drawRepeaterConfigMenu(int selectedIndex, const String& ssid, const String& pass, bool isWpa2, bool isHidden);
    void drawSavedNetworksMenu(int selectedIndex, int total, const String& ssidName);
    void drawDeleteConfirmMenu(const String& ssidName, bool isYesSelected);

    void drawOSDCustomTextEditor(const char* text, int kx, int ky);
    void drawOSDRunningText(const char* text, int scrollOffset, uint16_t speedMs, bool repeatMode, uint8_t spacingPx, bool paused, bool centeredMode);
    void drawOSDEyeAnimationStub(uint8_t blinkPhase);
    void drawSettingsMenu(int selectedIndex);
    void drawScreenSleepMenu(bool enabled, uint32_t timeoutMs);
    void setDisplaySleep(bool enabled);
    Adafruit_SSD1306* getDisplay();

    /**
     * @brief Show "SENDING..." overlay for IR transmission.
     */
    void showSending();

    /**
     * @brief Render the initial boot splash animation.
     */
    void showBootAnimation();
};

extern DisplayManager displayManager;

#endif // DISPLAY_MANAGER_H
