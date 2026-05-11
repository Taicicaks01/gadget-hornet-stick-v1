#include "SafeMode.h"
#include <LittleFS.h>
#include "ButtonManager.h"
#include "DisplayManager.h"

SafeModeClass safeMode;

bool SafeModeClass::begin() {
    // Detect button held at boot
    // Assumes ButtonManager was initialized before calling this
    if (buttonManager.isLeftPressed()) {
        active = true;
    }
    return true;
}

bool SafeModeClass::isSafeMode() const { return active; }

void SafeModeClass::enter() {
    active = true;
    // Minimal OLED UI
    displayManager.showMessage("SAFE MODE");
}

void SafeModeClass::exit() {
    active = false;
}

void SafeModeClass::runDiagnostics() {
    // Attempt lightweight FS check
    if (!LittleFS.begin()) {
        displayManager.showMessage("FS Mount Fail");
        return;
    }
    displayManager.showMessage("FS OK");
}
