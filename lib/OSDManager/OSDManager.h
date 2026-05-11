#ifndef OSD_MANAGER_H
#define OSD_MANAGER_H

#include <Arduino.h>

enum class OSDScreen : uint8_t {
    Menu,
    CustomText,
    RunningText,
    EyeAnimation,
    SettingsMenu,
    ScreenSleep
};

struct OSDDisplayConfig {
    bool sleepEnabled;
    uint32_t sleepTimeoutMs;
    uint16_t scrollSpeedMs;
    bool repeatMode;
    uint8_t spacingPx;
    bool centeredMode;
};

class OSDManager {
private:
    OSDScreen currentScreen;
    OSDDisplayConfig config;

    int menuIndex;
    int editorX;
    int editorY;
    int runningFocus;
    bool runningPaused;
    bool customDirty;
    bool configDirty;
    bool blinkState;

    unsigned long lastRunningTick;
    unsigned long lastBlinkTick;
    int scrollOffset;
    bool runningNeedsFullClear;

    char customText[128];
    char editBuffer[128];

    void copyText(char* target, size_t targetSize, const char* source);
    void loadDefaults();
    void loadConfig();
    void saveConfig();
    void loadCustomText();
    void saveCustomText();
    static char keyboardChar(int x, int y);
    int runningTextWidth() const;

public:
    OSDManager();

    void begin();

    void onEnterMenu();
    void onEnterCustomText();
    void onEnterRunningText();
    void onExitRunningText();
    void onEnterEyeAnimation();
    void onEnterSettingsMenu();
    void onEnterScreenSleep();
    void onExit();

    void update();
    void render();

    void menuUp();
    void menuDown();
    int getMenuIndex() const;

    void editorUp();
    void editorDown();
    void editorLeft();
    void editorRight();
    void editorInsert();
    void editorBackspace();
    bool editorHoldSave();

    void runningTogglePause();
    void runningToggleRepeat();
    void runningToggleCentered();
    void runningSpeedUp();
    void runningSpeedDown();
    void runningSpacingUp();
    void runningSpacingDown();
    void runningAdjustSelected(bool increase);
    void runningCycleFocus();
    void runningStop();

    void sleepToggleEnabled();
    void sleepTimeoutUp();
    void sleepTimeoutDown();

    const char* getCustomText() const;
    bool isSleepEnabled() const;
    uint32_t getSleepTimeoutMs() const;
    bool shouldKeepScreenAwake() const;
    OSDScreen getCurrentScreen() const;
};

extern OSDManager osdManager;

#endif // OSD_MANAGER_H