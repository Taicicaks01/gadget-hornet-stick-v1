#include "AppManager.h"
#include <Arduino.h>

AppManagerClass appManager;

bool AppManagerClass::begin() {
    appCount = 0;
    current = nullptr;
    return true;
}

bool AppManagerClass::registerApp(const AppInterface* app) {
    if (appCount >= MAX_APPS) return false;
    apps[appCount++] = app;
    return true;
}

bool AppManagerClass::switchTo(AppId_t id) {
    // Special: id == 0xFF means stop active app
    if (id == 0xFF) {
        if (current && current->onExit) current->onExit();
        current = nullptr;
        return true;
    }

    if (current && current->id == id) return true;
    // find app
    const AppInterface* next = nullptr;
    for (int i = 0; i < appCount; ++i) {
        if (apps[i]->id == id) { next = apps[i]; break; }
    }
    if (!next) return false;
    if (current && current->onExit) current->onExit();
    current = next;
    if (current->onEnter) current->onEnter();
    return true;
}

bool AppManagerClass::stopActive() {
    if (current && current->onExit) current->onExit();
    current = nullptr;
    return true;
}

const AppInterface* AppManagerClass::getActiveApp() const {
    return current;
}

AppId_t AppManagerClass::activeAppId() const {
    return current ? current->id : 0xFF;
}

void AppManagerClass::update() {
    if (current && current->update) current->update();
}

void AppManagerClass::render() {
    if (current && current->render) current->render();
}
