#pragma once
#include <Arduino.h>

struct SystemInfoReport {
    uint32_t freeRam;
    uint32_t flashTotal;
    uint32_t flashUsed;
    uint32_t uptimeSec;
};

class SystemInfoClass {
public:
    bool begin();
    SystemInfoReport getReport();
};

extern SystemInfoClass systemInfo;
