#pragma once
#include <Arduino.h>

typedef uint8_t AppId_t;

// App lifecycle callbacks
typedef void (*AppCallback)();

struct AppInterface {
    AppId_t id;
    const char* name;
    AppCallback onEnter;
    AppCallback update;
    AppCallback render;
    AppCallback onExit;
};

class AppManagerClass {
public:
    bool begin();
    bool registerApp(const AppInterface* app);
    bool switchTo(AppId_t id);
    bool stopActive();
    const AppInterface* getActiveApp() const;
    void update();
    void render();
    AppId_t activeAppId() const;
private:
    static const int MAX_APPS = 12;
    const AppInterface* apps[MAX_APPS];
    int appCount = 0;
    const AppInterface* current = nullptr;
};

extern AppManagerClass appManager;
