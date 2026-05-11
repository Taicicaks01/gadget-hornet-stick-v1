#pragma once
#include <Arduino.h>

class BootManagerClass {
public:
    bool begin();
    bool runChecks();
    bool isHealthy() const;
private:
    bool healthy = false;
};

extern BootManagerClass bootManager;
