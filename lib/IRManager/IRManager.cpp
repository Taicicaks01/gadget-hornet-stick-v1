#include "IRManager.h"
#include "config.h"

const CommonIRCode commonCodes[] = {
    {"LG Power", NEC, 0x20DF10EF, 32},
    {"Samsung Power", SAMSUNG, 0xE0E040BF, 32},
    {"Sony Power", SONY, 0xA90, 12},
    {"Panasonic Power", PANASONIC, 0x40040100BCBD, 48},
    {"TCL Power", NEC, 0x00FF807F, 32}, // NEC-based
    {"Toshiba Power", NEC, 0x02FD48B7, 32}
};
const int commonCodesSize = sizeof(commonCodes) / sizeof(commonCodes[0]);

IRManager irManager;

IRManager::IRManager() : irrecv(IR_RECEIVE_PIN), irsend(IR_SEND_PIN), hasValidIR(false), lastIRCodeStr("Belum Ada"), lastIRProtocolStr("-") {}

void IRManager::init() {
    irrecv.enableIRIn();
    irsend.begin();
}

void IRManager::update() {
    if (irrecv.decode(&results)) {
        // Abaikan noise dan repeat
        if (results.decode_type != UNKNOWN && results.value != 0xFFFFFFFF && results.value != 0) {
            lastIRCodeStr = "0x" + uint64ToString(results.value, HEX);
            lastIRProtocolStr = typeToString(results.decode_type);
            
            lastValidIR = results;
            hasValidIR = true;

            Serial.print("IR Masuk: ");
            Serial.print(lastIRCodeStr);
            Serial.print(" | Protokol: ");
            Serial.println(lastIRProtocolStr);
        }
        irrecv.resume();
    }
}

void IRManager::transmitLastCode() {
    if (hasValidIR) {
        Serial.println("Mengirim ulang sinyal IR...");
        irsend.send(lastValidIR.decode_type, lastValidIR.value, lastValidIR.bits);
    } else {
        Serial.println("Belum ada sinyal IR yang tersimpan.");
    }
}

String IRManager::getLastCode() const { return lastIRCodeStr; }
String IRManager::getLastProtocol() const { return lastIRProtocolStr; }
bool IRManager::hasValidSignal() const { return hasValidIR; }

bool IRManager::saveToFile(const String& filename) {
    if (!hasValidIR) return false;
    File f = LittleFS.open("/" + filename + ".txt", "w");
    if (!f) return false;
    f.println(lastValidIR.decode_type);
    f.println(uint64ToString(lastValidIR.value, 10)); // Save as base10 string
    f.println(lastValidIR.bits);
    f.close();
    return true;
}

bool IRManager::loadAndSend(const String& filename) {
    File f = LittleFS.open("/" + filename, "r");
    if (!f) return false;
    
    String typeStr = f.readStringUntil('\n');
    String valStr = f.readStringUntil('\n');
    String bitsStr = f.readStringUntil('\n');
    f.close();

    decode_type_t type = (decode_type_t)typeStr.toInt();
    uint64_t val = strtoull(valStr.c_str(), NULL, 10);
    uint16_t bits = bitsStr.toInt();

    irsend.send(type, val, bits);
    return true;
}

bool IRManager::deleteFile(const String& filename) {
    return LittleFS.remove("/" + filename);
}

int IRManager::getSavedFilesCount() {
    Dir dir = LittleFS.openDir("/");
    int count = 0;
    while (dir.next()) {
        if(dir.isFile() && dir.fileName().endsWith(".txt")) {
            count++;
        }
    }
    return count;
}

String IRManager::getFileName(int index) {
    Dir dir = LittleFS.openDir("/");
    int count = 0;
    while (dir.next()) {
        if(dir.isFile() && dir.fileName().endsWith(".txt")) {
            if(count == index) {
                return dir.fileName();
            }
            count++;
        }
    }
    return "";
}

void IRManager::sendBruteforceCode(int index) {
    if(index < commonCodesSize) {
        irsend.send(commonCodes[index].type, commonCodes[index].value, commonCodes[index].bits);
    } else {
        int fileIndex = index - commonCodesSize;
        String fName = getFileName(fileIndex);
        if(fName != "") {
            loadAndSend(fName);
        }
    }
}
