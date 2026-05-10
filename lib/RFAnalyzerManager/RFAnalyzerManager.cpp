#include "RFAnalyzerManager.h"

RFAnalyzerManager rfManager;

struct sniffer_buf2 {
    struct {
        signed rssi:8;
        unsigned rate:4;
        unsigned is_group:1;
        unsigned:1;
        unsigned sig_mode:2;
        unsigned legacy_length:12;
        unsigned damatch0:1;
        unsigned damatch1:1;
        unsigned bssidmatch0:1;
        unsigned bssidmatch1:1;
        unsigned MCS:7;
        unsigned CWB:1;
        unsigned HT_length:16;
        unsigned Smoothing:1;
        unsigned Not_Sounding:1;
        unsigned:1;
        unsigned Aggregation:1;
        unsigned STBC:2;
        unsigned FEC_CODING:1;
        unsigned SGI:1;
        unsigned rxend_state:8;
        unsigned ampdu_cnt:8;
        unsigned channel:4;
        unsigned:12;
    } rx_ctrl;
    uint8_t buf[112];
    uint16_t cnt;
    uint16_t len;
};

void RFAnalyzerManager::snifferCallback(uint8_t *buf, uint16_t len) {
    if (len == 128) { // standard packet
        struct sniffer_buf2 *sniffer = (struct sniffer_buf2 *)buf;
        int rssi = sniffer->rx_ctrl.rssi;
        uint8_t frameControl = sniffer->buf[0];
        uint8_t type = (frameControl & 0x0C) >> 2;

        rfManager.rssiSum += rssi;
        rfManager.rssiSamples++;
        rfManager.packetCount++;

        if (type == 0) rfManager.mgmtCount++;
        else if (type == 1) rfManager.ctrlCount++;
        else if (type == 2) rfManager.dataCount++;
        
        // Very basic MAC/SSID extraction for beacon flood could go here, 
        // but it's kept minimal to save RAM.
    } else {
        // Different packet structure (e.g. 12 byte offset)
        int rssi = (signed char)buf[0]; // approximated for brevity
        rfManager.rssiSum += rssi;
        rfManager.rssiSamples++;
        rfManager.packetCount++;
    }
}

RFAnalyzerManager::RFAnalyzerManager() : 
    currentMode(ANALYZER_MODE_OFF), currentChannel(1), lastChannelSwitch(0),
    packetCount(0), mgmtCount(0), dataCount(0), ctrlCount(0), rssiSum(0), rssiSamples(0),
    lastPPS(0), lastMgmtPPS(0), lastDataPPS(0), lastCtrlPPS(0), avgRSSI(-100), peakRSSI(-100),
    lastStatsUpdate(0), lastAutoLogTime(0), ssidIndex(0), uniqueSSIDCount(0) {
    for(int i=0; i<13; i++) heatmapRSSI[i] = -100;
}

void RFAnalyzerManager::init() {
    // Just ensure WiFi is ready to be configured
    WiFi.mode(WIFI_OFF);
}

void RFAnalyzerManager::start(AnalyzerMode mode) {
    currentMode = mode;
    WiFi.mode(WIFI_STA);
    wifi_set_opmode(STATION_MODE);
    wifi_promiscuous_enable(0);
    WiFi.disconnect();
    wifi_set_promiscuous_rx_cb(snifferCallback);
    wifi_set_channel(currentChannel);
    wifi_promiscuous_enable(1);
    
    // Reset counters
    packetCount = 0; mgmtCount = 0; dataCount = 0; ctrlCount = 0;
    rssiSum = 0; rssiSamples = 0;
    avgRSSI = -100; peakRSSI = -100; lastPPS = 0;
    lastStatsUpdate = millis();
}

void RFAnalyzerManager::stop() {
    wifi_promiscuous_enable(0);
    WiFi.mode(WIFI_OFF);
    currentMode = ANALYZER_MODE_OFF;
}

void RFAnalyzerManager::changeChannel(uint8_t channel) {
    currentChannel = channel;
    wifi_set_channel(currentChannel);
}

void RFAnalyzerManager::update() {
    if (currentMode == ANALYZER_MODE_OFF) return;

    unsigned long now = millis();
    
    // Update Stats every second
    if (now - lastStatsUpdate >= 1000) {
        lastPPS = packetCount;
        lastMgmtPPS = mgmtCount;
        lastDataPPS = dataCount;
        lastCtrlPPS = ctrlCount;
        
        if (rssiSamples > 0) {
            int currentAvg = rssiSum / rssiSamples;
            // EMA Smoothing
            if(avgRSSI == -100) avgRSSI = currentAvg;
            else avgRSSI = (currentAvg * 3 + avgRSSI * 7) / 10;
            
            if(currentAvg > peakRSSI) peakRSSI = currentAvg;
        } else {
            avgRSSI = -100;
        }

        // Decay peak
        if (peakRSSI > -100 && now % 5000 < 1000) peakRSSI--;

        if (currentMode == ANALYZER_MODE_HEATMAP) {
            heatmapRSSI[currentChannel - 1] = avgRSSI;
        }
        
        // Auto Log Events
        if (currentMode != ANALYZER_MODE_HEATMAP && lastPPS > 500) {
            appendLogEvent("HIGH_TRAFFIC", "PPS:" + String(lastPPS) + " RSSI:" + String(avgRSSI));
        }

        // Reset volatile counters
        packetCount = 0; mgmtCount = 0; dataCount = 0; ctrlCount = 0;
        rssiSum = 0; rssiSamples = 0;
        lastStatsUpdate = now;
    }

    // Channel Hopping for Heatmap
    if (currentMode == ANALYZER_MODE_HEATMAP) {
        if (now - lastChannelSwitch >= 200) {
            currentChannel++;
            if (currentChannel > 13) currentChannel = 1;
            changeChannel(currentChannel);
            lastChannelSwitch = now;
        }
    }
}

int RFAnalyzerManager::getRSSI() const { return avgRSSI; }
int RFAnalyzerManager::getPeakRSSI() const { return peakRSSI; }
int RFAnalyzerManager::getPPS() const { return lastPPS; }
int RFAnalyzerManager::getMgmtPPS() const { return lastMgmtPPS; }
int RFAnalyzerManager::getDataPPS() const { return lastDataPPS; }
int RFAnalyzerManager::getCtrlPPS() const { return lastCtrlPPS; }
int RFAnalyzerManager::getUniqueSSIDCount() const { return uniqueSSIDCount; }
int* RFAnalyzerManager::getHeatmap() { return heatmapRSSI; }
uint8_t RFAnalyzerManager::getCurrentChannel() const { return currentChannel; }

String RFAnalyzerManager::getStatusString() {
    if (lastPPS > 800 || lastMgmtPPS > 100) return "CHAOTIC";
    if (lastPPS > 200) return "BUSY";
    return "CLEAN";
}

void RFAnalyzerManager::appendLogEvent(String eventType, String details) {
    if(millis() - lastAutoLogTime < 5000) return; // Prevent log flooding (max 1 every 5s)
    
    // Simple file log
    File f = LittleFS.open("/rf_event.txt", "a");
    if (f) {
        if(f.size() > 20000) { // Rotate if > 20KB
            f.close();
            LittleFS.remove("/rf_event.txt");
            f = LittleFS.open("/rf_event.txt", "a");
        }
        f.printf("T:%lu E:%s D:%s\n", millis(), eventType.c_str(), details.c_str());
        f.close();
        lastAutoLogTime = millis();
    }
}
