#pragma once
#include <Arduino.h>

class GlobalLoggerClass {
public:
    bool begin();
    bool append(const char* msg);
    void rotateIfNeeded();
private:
    const char* logDir = "/logs";
    const char* logFile = "/logs/log.txt";
    const size_t MAX_LOG_SIZE = 8 * 1024; // 8KB
};

extern GlobalLoggerClass globalLogger;
