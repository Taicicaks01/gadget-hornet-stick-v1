#include "BootManager.h"
#include <LittleFS.h>
#include "DisplayManager.h"
#include "ButtonManager.h"
#include "EventManager.h"
#include "GlobalLogger.h"
#include "SafeMode.h"
#include "FileExplorer.h"

BootManagerClass bootManager;

bool BootManagerClass::begin() {
    healthy = false;
    return true;
}

bool BootManagerClass::runChecks() {
    // Quick non-blocking checks
    bool ok = true;
    if (!LittleFS.begin()) {
        globalLogger.append("[BOOT] LittleFS mount failed");
        ok = false;
    } else {
        globalLogger.append("[BOOT] LittleFS OK");
    }

    // Buttons
    // ButtonManager assumed initialized in setup
    if (buttonManager.isUpPressed() || buttonManager.isDownPressed() || buttonManager.isLeftPressed() || buttonManager.isRightPressed() || buttonManager.isOKPressed()) {
        globalLogger.append("[BOOT] Button pressed at boot");
    }

    if (safeMode.isSafeMode()) {
        globalLogger.append("[BOOT] Safe Mode requested");
        ok = false; // treat safe-mode as non-normal
    }

    healthy = ok;
    return healthy;
}

bool BootManagerClass::isHealthy() const { return healthy; }
