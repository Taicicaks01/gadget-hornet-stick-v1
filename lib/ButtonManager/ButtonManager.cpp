#include "ButtonManager.h"
#include "config.h"

ButtonManager buttonManager;

ButtonManager::ButtonManager() : 
    okPressedLast(false), upPressedLast(false), downPressedLast(false), 
    leftPressedLast(false), rightPressedLast(false),
    lastDebounceOK(0), lastDebounceUp(0), lastDebounceDown(0), 
    lastDebounceLeft(0), lastDebounceRight(0) {}

void ButtonManager::init() {
    pinMode(BTN_UP, INPUT); // D0 needs external pullup
    pinMode(BTN_DOWN, INPUT_PULLUP);
    pinMode(BTN_OK, INPUT_PULLUP);
    pinMode(BTN_LEFT, INPUT_PULLUP);
    pinMode(BTN_RIGHT, INPUT_PULLUP);
}

bool ButtonManager::isUpPressed() const { return digitalRead(BTN_UP) == LOW; }
bool ButtonManager::isDownPressed() const { return digitalRead(BTN_DOWN) == LOW; }
bool ButtonManager::isLeftPressed() const { return digitalRead(BTN_LEFT) == LOW; }
bool ButtonManager::isRightPressed() const { return digitalRead(BTN_RIGHT) == LOW; }
bool ButtonManager::isOKPressed() const { return digitalRead(BTN_OK) == LOW; }

bool ButtonManager::isOKJustPressed() {
    bool current = isOKPressed();
    if (current && !okPressedLast && (millis() - lastDebounceOK > debounceDelay)) {
        lastDebounceOK = millis();
        okPressedLast = true;
        return true;
    }
    if (!current) okPressedLast = false;
    return false;
}

bool ButtonManager::isUpJustPressed() {
    bool current = isUpPressed();
    if (current && !upPressedLast && (millis() - lastDebounceUp > debounceDelay)) {
        lastDebounceUp = millis();
        upPressedLast = true;
        return true;
    }
    if (!current) upPressedLast = false;
    return false;
}

bool ButtonManager::isDownJustPressed() {
    bool current = isDownPressed();
    if (current && !downPressedLast && (millis() - lastDebounceDown > debounceDelay)) {
        lastDebounceDown = millis();
        downPressedLast = true;
        return true;
    }
    if (!current) downPressedLast = false;
    return false;
}

bool ButtonManager::isLeftJustPressed() {
    bool current = isLeftPressed();
    if (current && !leftPressedLast && (millis() - lastDebounceLeft > debounceDelay)) {
        lastDebounceLeft = millis();
        leftPressedLast = true;
        return true;
    }
    if (!current) leftPressedLast = false;
    return false;
}

bool ButtonManager::isRightJustPressed() {
    bool current = isRightPressed();
    if (current && !rightPressedLast && (millis() - lastDebounceRight > debounceDelay)) {
        lastDebounceRight = millis();
        rightPressedLast = true;
        return true;
    }
    if (!current) rightPressedLast = false;
    return false;
}
