#include "OSDManager.h"

#include <LittleFS.h>
#include <string.h>

#include "DisplayManager.h"
#include "config.h"

OSDManager osdManager;

static const char* OSD_CONFIG_FILE = "/config/display.cfg";
static const char* OSD_TEXT_FILE = "/config/osd_text.txt";

OSDManager::OSDManager() :
    currentScreen(OSDScreen::Menu),
    config{true, 30000, 80, true, 12, false},
    menuIndex(0), editorX(0), editorY(0), runningFocus(0),
    runningPaused(false), customDirty(false), configDirty(false), blinkState(false),
    lastRunningTick(0), lastBlinkTick(0), scrollOffset(0), runningNeedsFullClear(false) {
    loadDefaults();
}

void OSDManager::copyText(char* target, size_t targetSize, const char* source) {
    if (!target || targetSize == 0) {
        return;
    }
    if (!source) {
        target[0] = '\0';
        return;
    }
    strncpy(target, source, targetSize - 1);
    target[targetSize - 1] = '\0';
}

void OSDManager::loadDefaults() {
    config.sleepEnabled = true;
    config.sleepTimeoutMs = 30000;
    config.scrollSpeedMs = 80;
    config.repeatMode = true;
    config.spacingPx = 12;
    config.centeredMode = false;
    copyText(customText, sizeof(customText), "GADGET V1\nONLINE");
    copyText(editBuffer, sizeof(editBuffer), customText);
}

void OSDManager::loadConfig() {
    loadDefaults();

    File file = LittleFS.open(OSD_CONFIG_FILE, "r");
    if (!file) {
        return;
    }

    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        if (line.startsWith("SLEEP=")) {
            config.sleepEnabled = line.substring(6).toInt() != 0;
        } else if (line.startsWith("TIMEOUT=")) {
            unsigned long timeoutValue = static_cast<unsigned long>(line.substring(8).toInt());
            if (timeoutValue < 5000UL) {
                timeoutValue = 5000UL;
            }
            config.sleepTimeoutMs = timeoutValue;
        } else if (line.startsWith("RT_SPEED=")) {
            config.scrollSpeedMs = constrain(line.substring(9).toInt(), 20, 500);
        } else if (line.startsWith("RT_REPEAT=")) {
            config.repeatMode = line.substring(10).toInt() != 0;
        } else if (line.startsWith("RT_SPACE=")) {
            config.spacingPx = constrain(line.substring(9).toInt(), 0, 64);
        } else if (line.startsWith("RT_CENTER=")) {
            config.centeredMode = line.substring(10).toInt() != 0;
        }
    }
    file.close();
}

void OSDManager::saveConfig() {
    LittleFS.mkdir("/config");

    File file = LittleFS.open(OSD_CONFIG_FILE, "w");
    if (!file) {
        return;
    }

    file.print("SLEEP=");
    file.println(config.sleepEnabled ? 1 : 0);
    file.print("TIMEOUT=");
    file.println(config.sleepTimeoutMs);
    file.print("RT_SPEED=");
    file.println(config.scrollSpeedMs);
    file.print("RT_REPEAT=");
    file.println(config.repeatMode ? 1 : 0);
    file.print("RT_SPACE=");
    file.println(config.spacingPx);
    file.print("RT_CENTER=");
    file.println(config.centeredMode ? 1 : 0);
    file.close();
    configDirty = false;
}

void OSDManager::loadCustomText() {
    copyText(editBuffer, sizeof(editBuffer), customText);

    File file = LittleFS.open(OSD_TEXT_FILE, "r");
    if (!file) {
        return;
    }

    size_t len = file.readBytes(editBuffer, sizeof(editBuffer) - 1);
    editBuffer[len] = '\0';
    file.close();
    copyText(customText, sizeof(customText), editBuffer);
}

void OSDManager::saveCustomText() {
    LittleFS.mkdir("/config");

    File file = LittleFS.open(OSD_TEXT_FILE, "w");
    if (!file) {
        return;
    }

    file.print(editBuffer);
    file.close();
    copyText(customText, sizeof(customText), editBuffer);
    customDirty = false;
}

char OSDManager::keyboardChar(int x, int y) {
    static const char keys[3][13] = {
        "ABCDEFGHIJKL",
        "MNOPQRSTUVWX",
        "YZ0123456789"
    };
    if (x < 0 || x > 11 || y < 0 || y > 2) {
        return ' ';
    }
    return keys[y][x];
}

int OSDManager::runningTextWidth() const {
    return static_cast<int>(strlen(editBuffer) * 6);
}

void OSDManager::begin() {
    loadConfig();
    loadCustomText();
}

void OSDManager::onEnterMenu() {
    currentScreen = OSDScreen::Menu;
    menuIndex = 0;
}

void OSDManager::onEnterCustomText() {
    currentScreen = OSDScreen::CustomText;
    editorX = 0;
    editorY = 0;
    customDirty = false;
    copyText(editBuffer, sizeof(editBuffer), customText);
}

void OSDManager::onEnterRunningText() {
    currentScreen = OSDScreen::RunningText;
    runningPaused = false;
    runningFocus = 0;
    lastRunningTick = millis();
    scrollOffset = SCREEN_WIDTH;
    runningNeedsFullClear = true;
    copyText(editBuffer, sizeof(editBuffer), customText);
}

void OSDManager::onExitRunningText() {
    runningPaused = false;
    runningFocus = 0;
    scrollOffset = SCREEN_WIDTH;
    runningNeedsFullClear = true;
    currentScreen = OSDScreen::Menu;
}

void OSDManager::onEnterEyeAnimation() {
    currentScreen = OSDScreen::EyeAnimation;
    lastBlinkTick = millis();
    blinkState = false;
}

void OSDManager::onEnterSettingsMenu() {
    currentScreen = OSDScreen::SettingsMenu;
}

void OSDManager::onEnterScreenSleep() {
    currentScreen = OSDScreen::ScreenSleep;
}

void OSDManager::onExit() {
    if (customDirty) {
        saveCustomText();
    }
    if (configDirty) {
        saveConfig();
    }
    currentScreen = OSDScreen::Menu;
}

void OSDManager::update() {
    if (currentScreen == OSDScreen::RunningText) {
        if (runningPaused) {
            return;
        }
        if (millis() - lastRunningTick < config.scrollSpeedMs) {
            return;
        }
        lastRunningTick = millis();

        int textWidth = runningTextWidth();
        scrollOffset -= 1;
        if (scrollOffset < -textWidth - config.spacingPx) {
            scrollOffset = SCREEN_WIDTH;
        }
    } else if (currentScreen == OSDScreen::EyeAnimation) {
        if (millis() - lastBlinkTick > 450) {
            lastBlinkTick = millis();
            blinkState = !blinkState;
        }
    }
}

void OSDManager::render() {
    switch (currentScreen) {
        case OSDScreen::Menu:
            displayManager.drawOSDMenu(menuIndex);
            break;
        case OSDScreen::CustomText:
            displayManager.drawOSDCustomTextEditor(editBuffer, editorX, editorY);
            break;
        case OSDScreen::RunningText:
            if (runningNeedsFullClear) {
                displayManager.clear();
                runningNeedsFullClear = false;
            }
            displayManager.drawOSDRunningText(editBuffer, scrollOffset, config.scrollSpeedMs, config.repeatMode, config.spacingPx, runningPaused, false);
            break;
        case OSDScreen::EyeAnimation:
            displayManager.drawOSDEyeAnimationStub(blinkState ? 1 : 0);
            break;
        case OSDScreen::SettingsMenu:
            displayManager.drawSettingsMenu(0);
            break;
        case OSDScreen::ScreenSleep:
            displayManager.drawScreenSleepMenu(config.sleepEnabled, config.sleepTimeoutMs);
            break;
    }
}

void OSDManager::menuUp() {
    if (menuIndex <= 0) {
        menuIndex = 2;
    } else {
        menuIndex--;
    }
}

void OSDManager::menuDown() {
    if (menuIndex >= 2) {
        menuIndex = 0;
    } else {
        menuIndex++;
    }
}

int OSDManager::getMenuIndex() const {
    return menuIndex;
}

void OSDManager::editorUp() {
    if (editorY <= 0) {
        editorY = 2;
    } else {
        editorY--;
    }
}

void OSDManager::editorDown() {
    if (editorY >= 2) {
        editorY = 0;
    } else {
        editorY++;
    }
}

void OSDManager::editorLeft() {
    if (editorX <= 0) {
        editorX = 11;
    } else {
        editorX--;
    }
}

void OSDManager::editorRight() {
    if (editorX >= 11) {
        editorX = 0;
    } else {
        editorX++;
    }
}

void OSDManager::editorInsert() {
    char c = keyboardChar(editorX, editorY);
    size_t len = strlen(editBuffer);
    if (len >= sizeof(editBuffer) - 1) {
        return;
    }
    editBuffer[len] = c;
    editBuffer[len + 1] = '\0';
    customDirty = true;
}

void OSDManager::editorBackspace() {
    size_t len = strlen(editBuffer);
    if (len == 0) {
        return;
    }
    editBuffer[len - 1] = '\0';
    customDirty = true;
}

bool OSDManager::editorHoldSave() {
    if (!customDirty) {
        return false;
    }
    saveCustomText();
    return true;
}

void OSDManager::runningTogglePause() {
    runningPaused = !runningPaused;
}

void OSDManager::runningToggleRepeat() {
    config.repeatMode = !config.repeatMode;
    configDirty = true;
}

void OSDManager::runningToggleCentered() {
    config.centeredMode = !config.centeredMode;
    configDirty = true;
    if (config.centeredMode) {
        int textWidth = runningTextWidth();
        scrollOffset = (SCREEN_WIDTH - textWidth) / 2;
    } else {
        scrollOffset = SCREEN_WIDTH;
    }
}

void OSDManager::runningSpeedUp() {
    if (config.scrollSpeedMs > 20) {
        config.scrollSpeedMs -= 10;
        configDirty = true;
    }
}

void OSDManager::runningSpeedDown() {
    if (config.scrollSpeedMs < 500) {
        config.scrollSpeedMs += 10;
        configDirty = true;
    }
}

void OSDManager::runningSpacingUp() {
    if (config.spacingPx < 64) {
        config.spacingPx += 2;
        configDirty = true;
    }
}

void OSDManager::runningSpacingDown() {
    if (config.spacingPx > 0) {
        config.spacingPx -= 2;
        configDirty = true;
    }
}

void OSDManager::runningAdjustSelected(bool increase) {
    switch (runningFocus) {
        case 0:
            if (increase) {
                runningSpeedUp();
            } else {
                runningSpeedDown();
            }
            break;
        case 1:
            if (increase) {
                runningSpacingUp();
            } else {
                runningSpacingDown();
            }
            break;
        case 2:
            runningToggleRepeat();
            break;
        case 3:
            runningToggleCentered();
            break;
    }
}

void OSDManager::runningCycleFocus() {
    runningFocus++;
    if (runningFocus > 3) {
        runningFocus = 0;
    }
}

void OSDManager::runningStop() {
    runningPaused = false;
    scrollOffset = SCREEN_WIDTH;
}

void OSDManager::sleepToggleEnabled() {
    config.sleepEnabled = !config.sleepEnabled;
    configDirty = true;
}

void OSDManager::sleepTimeoutUp() {
    config.sleepTimeoutMs = min(config.sleepTimeoutMs + 5000UL, 120000UL);
    configDirty = true;
}

void OSDManager::sleepTimeoutDown() {
    if (config.sleepTimeoutMs > 5000UL) {
        config.sleepTimeoutMs -= 5000UL;
        configDirty = true;
    }
}

const char* OSDManager::getCustomText() const {
    return editBuffer;
}

bool OSDManager::isSleepEnabled() const {
    return config.sleepEnabled;
}

uint32_t OSDManager::getSleepTimeoutMs() const {
    return config.sleepTimeoutMs;
}

bool OSDManager::shouldKeepScreenAwake() const {
    return currentScreen == OSDScreen::RunningText || currentScreen == OSDScreen::CustomText || currentScreen == OSDScreen::EyeAnimation;
}

OSDScreen OSDManager::getCurrentScreen() const {
    return currentScreen;
}