#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <Arduino.h>

class ButtonManager {
private:
    bool okPressedLast;
    bool upPressedLast;
    bool downPressedLast;
    bool leftPressedLast;
    bool rightPressedLast;

    unsigned long lastDebounceOK;
    unsigned long lastDebounceUp;
    unsigned long lastDebounceDown;
    unsigned long lastDebounceLeft;
    unsigned long lastDebounceRight;
    
    const unsigned long debounceDelay = 150; // 150ms debounce time

public:
    ButtonManager();
    void init();
    
    // Simple state checking
    bool isUpPressed() const;
    bool isDownPressed() const;
    bool isLeftPressed() const;
    bool isRightPressed() const;
    bool isOKPressed() const;

    // Edge detection for triggers
    bool isOKJustPressed();
    bool isUpJustPressed();
    bool isDownJustPressed();
    bool isLeftJustPressed();
    bool isRightJustPressed();
};

extern ButtonManager buttonManager;

#endif // BUTTON_MANAGER_H
