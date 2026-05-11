#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class EyeAnimation {
public:
    enum Expression : uint8_t {
        Idle,
        Happy,
        Laugh,
        Sad,
        Angry,
        Thinking,
        Cool,
        Shock,
        Confused,
        Clown,
        Stone,
        Normal,
        Sleepy,
        Love,
        Wink,
        Dizzy,
        Bored
    };

    explicit EyeAnimation(Adafruit_SSD1306* display);
    void begin();
    void setExpression(Expression expression);
    void update();
    void render();

private:
    Adafruit_SSD1306* display;
    Expression currentExpression;
    uint8_t framePhase;
    uint8_t eyeClosePhase;
    unsigned long lastFrameMs;
    unsigned long lastIdleMoveMs;
    int leftX;
    int rightX;
    int eyeY;
    int eyeWidth;
    int eyeHeight;
    int pupilOffset;
    bool blinkOpen;
    bool heartMode;
    bool stoneMode;

    void renderIdle();
    void renderHappy();
    void renderLaugh();
    void renderSad();
    void renderAngry();
    void renderThinking();
    void renderCool();
    void renderShock();
    void renderConfused();
    void renderClown();
    void renderStone();
    void renderNormal();
    void renderSleepy();
    void renderLove();
    void renderWink();
    void renderDizzy();
    void renderBored();

    void drawEyes(int leftW, int leftH, int rightW, int rightH, int leftY, int rightY, int radius, int pupilX, int pupilY);
    void drawHeart(int x, int y, int size);
    void drawStoneFace();
};
