#ifndef WIFI_REPEATER_MANAGER_H
#define WIFI_REPEATER_MANAGER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <LittleFS.h>

struct APConfig {
    String ssidName;
    String password;
    bool isWPA2;
    bool isHidden;
};

/**
 * @class WiFiRepeaterManager
 * @brief Manages ESP8266 WiFi radio for AP+STA Repeater functionality.
 * 
 * This class implements a Network Address Port Translation (NAPT) stack 
 * and a DNS Proxy to allow multiple clients to share one upstream connection.
 * It includes a heuristic traffic estimator to provide real-time UI feedback.
 */
class WiFiRepeaterManager {
public:
    WiFiRepeaterManager();
    
    /**
     * @brief Initialize filesystem and load stored AP configurations.
     */
    void begin();

    /**
     * @brief Main update loop for non-blocking tasks (DNS proxy & Traffic Heuristics).
     * Must be called frequently in the main loop.
     */
    void update();

    /**
     * @brief Connects to an upstream WiFi network.
     * @param ssid The SSID of the target network.
     * @param pass The password of the target network.
     */
    void startRepeater(const String &ssid, const String &pass);

    /**
     * @brief Hard disconnect of all WiFi interfaces and powers down the radio.
     * Essential for freeing the radio for other modules (like RF Analyzer).
     */
    void stopRepeater();

    /**
     * @brief Initializes the LwIP NAPT stack and starts the DNS Proxy.
     * Should only be called once the STA interface is connected.
     */
    void configureNAPT();

    // --- AP Profile Management ---
    APConfig apConfig;
    void loadAPConfig();
    void saveAPConfig();
    void factoryResetAP();

    // --- Saved Networks Management ---
    bool hasSavedNetworks();
    int getSavedNetworksCount();
    String getSavedNetworkSSID(int index);
    bool loadNetwork(int index, String &ssid, String &pass);
    void saveNetwork(const String &ssid, const String &pass);
    void deleteNetwork(int index);

    // --- Interactive WiFi Scanner ---
    int scanNetworks();
    String getScannedSSID(int index);
    int getScannedRSSI(int index);
    bool getScannedEncryption(int index);

    // --- Runtime Status ---
    bool isConnected();
    String getLocalIP();
    int getRSSI();
    int getConnectedClients();

    // --- Heuristic Traffic Estimator Accessors ---
    uint64_t getRxBytes();
    uint64_t getTxBytes();
    uint32_t getDownSpeed();
    uint32_t getUpSpeed();

private:
    WiFiUDP dnsUdp;
    void startDnsProxy();
    void handleDnsRequest();

    // Internal Traffic State
    uint64_t rxBytesTotal;
    uint64_t txBytesTotal;
    uint32_t currentDownSpeed;
    uint32_t currentUpSpeed;
    unsigned long lastTrafficUpdate;
    int burstHits;
};

#endif
