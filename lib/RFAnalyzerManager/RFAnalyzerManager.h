#ifndef RFANALYZERMANAGER_H
#define RFANALYZERMANAGER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>

enum AnalyzerMode {
    ANALYZER_MODE_OFF,
    ANALYZER_MODE_LIVE,
    ANALYZER_MODE_DEAUTH,
    ANALYZER_MODE_BEACON,
    ANALYZER_MODE_SPIKE,
    ANALYZER_MODE_STATS,
    ANALYZER_MODE_HEATMAP,
    ANALYZER_MODE_RADAR
};

/**
 * @class RFAnalyzerManager
 * @brief Manages ESP8266 Promiscuous Mode for 2.4GHz WiFi analysis.
 * 
 * This class provides methods to capture raw WiFi packets, calculate PPS (Packets Per Second),
 * generate channel heatmaps, and perform event detection (Beacon Flood, etc).
 */
class RFAnalyzerManager {
private:
    AnalyzerMode currentMode;
    uint8_t currentChannel;
    unsigned long lastChannelSwitch;
    
    // Volatile counters for use in Sniffer Interrupt Callback
    volatile int packetCount;
    volatile int mgmtCount;
    volatile int dataCount;
    volatile int ctrlCount;
    volatile int rssiSum;
    volatile int rssiSamples;

    // Computed Stats
    int lastPPS;
    int lastMgmtPPS;
    int lastDataPPS;
    int lastCtrlPPS;
    int avgRSSI;
    int peakRSSI;
    
    unsigned long lastStatsUpdate;
    unsigned long lastAutoLogTime;
    
    int heatmapRSSI[13];
    
    // SSID Buffer for unique detection
    String lastSSIDs[10];
    int ssidIndex;
    int uniqueSSIDCount;

    void processPacket(uint8_t* buf, uint16_t len);

public:
    RFAnalyzerManager();
    
    /**
     * @brief Initialize the analyzer (pre-init tasks).
     */
    void init();

    /**
     * @brief Starts the sniffer in a specific mode.
     * @param mode The AnalyzerMode (Live, Deauth, Beacon, etc).
     */
    void start(AnalyzerMode mode);

    /**
     * @brief Safely stops the sniffer and restores normal WiFi functions.
     */
    void stop();

    /**
     * @brief Update method to handle channel hopping and stats calculation.
     */
    void update();

    /**
     * @brief Switch the hardware radio to a specific WiFi channel (1-13).
     */
    void changeChannel(uint8_t channel);
    
    // --- Data Accessors ---
    int getRSSI() const;
    int getPeakRSSI() const;
    int getPPS() const;
    int getMgmtPPS() const;
    int getDataPPS() const;
    int getCtrlPPS() const;
    int getUniqueSSIDCount() const;
    int* getHeatmap();
    uint8_t getCurrentChannel() const;
    String getStatusString();
    
    // --- Logging System ---
    void appendLogEvent(String eventType, String details);
    void checkAndRotateLogs();

    /**
     * @brief SDK Static Callback for raw packet injection.
     */
    static void snifferCallback(uint8_t *buf, uint16_t len);
};

extern RFAnalyzerManager rfManager;

#endif
