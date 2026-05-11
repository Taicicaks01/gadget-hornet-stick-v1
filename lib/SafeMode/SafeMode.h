#pragma once
#include <Arduino.h>

class SafeModeClass {
public:
    bool begin();
    bool isSafeMode() const;
    void enter();
    void exit();
    void runDiagnostics();
private:
    bool active = false;
};

extern SafeModeClass safeMode;
