#include "GlobalLogger.h"
#include <LittleFS.h>
#include <Arduino.h>

GlobalLoggerClass globalLogger;

bool GlobalLoggerClass::begin() {
    if (!LittleFS.begin()) return false;
    if (!LittleFS.exists(logDir)) {
        LittleFS.mkdir(logDir);
    }
    rotateIfNeeded();
    return true;
}

bool GlobalLoggerClass::append(const char* msg) {
    File f = LittleFS.open(logFile, "a");
    if (!f) return false;
    f.println(msg);
    f.close();
    rotateIfNeeded();
    return true;
}

void GlobalLoggerClass::rotateIfNeeded() {
    File f = LittleFS.open(logFile, "r");
    if (!f) return; // nothing
    size_t sz = f.size();
    f.close();
    if (sz <= MAX_LOG_SIZE) return;
    // rotate: rename to log.txt.1 (overwrite)
    const char* newName = "/logs/log.txt.1";
    if (LittleFS.exists(newName)) LittleFS.remove(newName);
    LittleFS.rename(logFile, newName);
}
