#ifndef IR_MANAGER_H
#define IR_MANAGER_H

#include <Arduino.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>
#include <IRsend.h>
#include <LittleFS.h>

struct CommonIRCode {
    const char* name;
    decode_type_t type;
    uint64_t value;
    uint16_t bits;
};

// Konstanta Array untuk Bruteforce
extern const CommonIRCode commonCodes[];
extern const int commonCodesSize;

class IRManager {
private:
    IRrecv irrecv;
    IRsend irsend;
    decode_results results;
    
    decode_results lastValidIR;
    bool hasValidIR;

    String lastIRCodeStr;
    String lastIRProtocolStr;

public:
    IRManager();
    void init();
    void update(); // Checks for new signals
    void transmitLastCode();

    String getLastCode() const;
    String getLastProtocol() const;
    bool hasValidSignal() const;

    // --- Manajemen File LittleFS ---
    bool saveToFile(const String& filename);
    bool loadAndSend(const String& filename);
    bool deleteFile(const String& filename);
    int getSavedFilesCount();
    String getFileName(int index);
    
    // --- Bruteforce ---
    void sendBruteforceCode(int index);
};

extern IRManager irManager;

#endif // IR_MANAGER_H
