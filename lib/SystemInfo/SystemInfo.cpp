#include "SystemInfo.h"
#include <LittleFS.h>
#ifdef ESP8266
#include <ESP.h>
#endif

SystemInfoClass systemInfo;

bool SystemInfoClass::begin() {
    return true;
}

SystemInfoReport SystemInfoClass::getReport() {
    SystemInfoReport r;
#ifdef ESP8266
    r.freeRam = ESP.getFreeHeap();
#else
    r.freeRam = 0;
#endif
    r.uptimeSec = millis() / 1000;
    // LittleFS stats (best-effort)
    r.flashTotal = 0;
    r.flashUsed = 0;
    #if defined(LittleFS) && !defined(ESP8266)
    // some platforms provide totalBytes/usedBytes
    // keep conservative defaults on ESP8266
    r.flashTotal = LittleFS.totalBytes();
    r.flashUsed = LittleFS.usedBytes();
    #endif
    return r;
}
