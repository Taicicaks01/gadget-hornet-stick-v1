#include "EyeAnimation.h"

static const uint8_t MOAI_BITMAP[] PROGMEM = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

EyeAnimation::EyeAnimation(Adafruit_SSD1306* display)
: display(display), currentExpression(Idle), framePhase(0), eyeClosePhase(0), lastFrameMs(0), lastIdleMoveMs(0), leftX(28), rightX(74), eyeY(20), eyeWidth(26), eyeHeight(18), pupilOffset(0), blinkOpen(true), heartMode(false), stoneMode(false) {}

void EyeAnimation::begin() {
    lastFrameMs = millis();
    lastIdleMoveMs = millis();
}

void EyeAnimation::setExpression(Expression expression) {
    currentExpression = expression;
    framePhase = 0;
    eyeClosePhase = 0;
    blinkOpen = true;
    heartMode = false;
    stoneMode = false;
    eyeY = 20;
    eyeWidth = 26;
    eyeHeight = 18;
    pupilOffset = 0;

    switch (expression) {
        case Happy:
            eyeHeight = 20;
            break;
        case Laugh:
            eyeHeight = 8;
            break;
        case Sad:
            eyeHeight = 12;
            eyeY = 26;
            break;
        case Angry:
            eyeHeight = 14;
            eyeY = 18;
            break;
        case Thinking:
            pupilOffset = 3;
            break;
        case Cool:
            eyeHeight = 10;
            eyeY = 24;
            break;
        case Shock:
            eyeWidth = 30;
            eyeHeight = 26;
            eyeY = 16;
            break;
        case Confused:
            pupilOffset = 5;
            break;
        case Clown:
            eyeHeight = 18;
            pupilOffset = 2;
            break;
        case Stone:
            stoneMode = true;
            break;
        case Sleepy:
            eyeHeight = 9;
            eyeY = 28;
            break;
        case Love:
            heartMode = true;
            break;
        case Wink:
            eyeHeight = 18;
            break;
        case Dizzy:
            eyeWidth = 30;
            eyeHeight = 14;
            eyeY = 22;
            pupilOffset = 7;
            break;
        case Bored:
            eyeHeight = 10;
            eyeY = 28;
            break;
        default:
            break;
    }
}

void EyeAnimation::update() {
    if (millis() - lastFrameMs < 50) {
        return;
    }
    lastFrameMs = millis();
    framePhase++;

    if (currentExpression == Idle || currentExpression == Normal || currentExpression == Bored) {
        if (millis() - lastIdleMoveMs > 900) {
            lastIdleMoveMs = millis();
            int drift = (framePhase & 0x01) ? 1 : -1;
            leftX = constrain(leftX + drift, 24, 34);
            rightX = constrain(rightX + drift, 70, 80);
        }
    }

    if (currentExpression == Wink) {
        blinkOpen = (framePhase % 16) > 2;
    }
}

void EyeAnimation::render() {
    switch (currentExpression) {
        case Happy: renderHappy(); break;
        case Laugh: renderLaugh(); break;
        case Sad: renderSad(); break;
        case Angry: renderAngry(); break;
        case Thinking: renderThinking(); break;
        case Cool: renderCool(); break;
        case Shock: renderShock(); break;
        case Confused: renderConfused(); break;
        case Clown: renderClown(); break;
        case Stone: renderStone(); break;
        case Normal: renderNormal(); break;
        case Sleepy: renderSleepy(); break;
        case Love: renderLove(); break;
        case Wink: renderWink(); break;
        case Dizzy: renderDizzy(); break;
        case Bored: renderBored(); break;
        case Idle:
        default:
            renderIdle();
            break;
    }
}

void EyeAnimation::drawEyes(int leftW, int leftH, int rightW, int rightH, int leftY, int rightY, int radius, int pupilX, int pupilY) {
    display->clearDisplay();
    display->fillRoundRect(leftX, leftY, leftW, leftH, radius, SSD1306_WHITE);
    display->fillRoundRect(rightX, rightY, rightW, rightH, radius, SSD1306_WHITE);

    if (leftH > 8) {
        display->fillCircle(leftX + leftW / 2 + pupilX, leftY + leftH / 2 + pupilY, 3, SSD1306_BLACK);
        display->fillCircle(rightX + rightW / 2 - pupilX, rightY + rightH / 2 + pupilY, 3, SSD1306_BLACK);
    }

    display->display();
}

void EyeAnimation::drawHeart(int x, int y, int size) {
    int halfSize = size / 2;
    int quarterSize = size / 4;
    display->fillCircle(x + quarterSize, y + quarterSize, quarterSize, SSD1306_WHITE);
    display->fillCircle(x + halfSize + quarterSize, y + quarterSize, quarterSize, SSD1306_WHITE);
    display->fillTriangle(x, y + quarterSize, x + size, y + quarterSize, x + halfSize, y + size, SSD1306_WHITE);
}

void EyeAnimation::drawStoneFace() {
    display->clearDisplay();
    display->drawBitmap(0, 0, MOAI_BITMAP, 8, 8, SSD1306_WHITE);
    display->display();
}

void EyeAnimation::renderIdle() { drawEyes(26, 18, 26, 18, 20, 20, 7, pupilOffset, 0); }
void EyeAnimation::renderHappy() { drawEyes(26, 20, 26, 20, 18, 18, 8, 0, -1); }
void EyeAnimation::renderLaugh() { drawEyes(26, 8, 26, 8, 26, 26, 6, 0, 0); }
void EyeAnimation::renderSad() { drawEyes(24, 12, 24, 12, 26, 26, 6, 0, 2); }
void EyeAnimation::renderAngry() { drawEyes(26, 14, 26, 14, 18, 18, 6, 2, 0); }
void EyeAnimation::renderThinking() { drawEyes(26, 18, 26, 18, 20, 20, 7, pupilOffset, -1); }
void EyeAnimation::renderCool() { drawEyes(26, 10, 26, 10, 24, 24, 5, 0, 0); }
void EyeAnimation::renderShock() { drawEyes(30, 26, 30, 26, 16, 16, 10, 0, 0); }
void EyeAnimation::renderConfused() { drawEyes(26, 16, 26, 16, 20, 20, 7, pupilOffset, 1); }
void EyeAnimation::renderClown() { drawEyes(26, 18, 26, 18, 20, 20, 8, 2, 0); }
void EyeAnimation::renderStone() { drawStoneFace(); }
void EyeAnimation::renderNormal() { drawEyes(26, 18, 26, 18, 20, 20, 7, 0, 0); }
void EyeAnimation::renderSleepy() { drawEyes(26, 9, 26, 9, 28, 28, 5, 0, 0); }
void EyeAnimation::renderLove() {
    display->clearDisplay();
    drawHeart(22, 20, 28);
    drawHeart(78, 20, 28);
    display->display();
}
void EyeAnimation::renderWink() {
    display->clearDisplay();
    display->fillRoundRect(24, 22, 28, blinkOpen ? 14 : 3, 6, SSD1306_WHITE);
    if (blinkOpen) {
        display->fillCircle(80, 29, 12, SSD1306_WHITE);
    } else {
        display->fillRoundRect(74, 28, 28, 3, 6, SSD1306_WHITE);
    }
    display->display();
}
void EyeAnimation::renderDizzy() { drawEyes(30, 14, 30, 14, 22, 22, 6, 7, 0); }
void EyeAnimation::renderBored() { drawEyes(26, 10, 26, 10, 28, 28, 5, 0, 0); }
