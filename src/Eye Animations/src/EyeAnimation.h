// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// 🎨 EYE ANIMATION ENGINE
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Using FluxGarage RoboEyes smooth animation
// Non-blocking, state-machine based
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

#ifndef EYEANIMATION_H
#define EYEANIMATION_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "BuzzerFX.h"

// ━━━━━━━━━━━━━━━━━━━━
// 🎭 MOOD CONSTANTS
// ━━━━━━━━━━━━━━━━━━━━
#define DEFAULT 0
#define TIRED 1
#define ANGRY 2
#define HAPPY 3

class EyeAnimation {
private:
  Adafruit_SSD1306* display;
  BuzzerFX* buzzer;
  
  // Display colors
  uint8_t BGCOLOR = 0;
  uint8_t MAINCOLOR = 1;
  
  // Screen dimensions
  int screenWidth = 128;
  int screenHeight = 64;
  int frameInterval = 20;
  unsigned long fpsTimer = 0;
  
  // Eye geometry
  int eyeLwidthCurrent = 36;
  int eyeLheightCurrent = 1;
  int eyeLwidthNext = 36;
  int eyeLheightNext = 36;
  int eyeLx, eyeLy, eyeLxNext, eyeLyNext;
  byte eyeLborderRadiusCurrent = 8;
  byte eyeLborderRadiusNext = 8;
  
  int eyeRwidthCurrent = 36;
  int eyeRheightCurrent = 1;
  int eyeRwidthNext = 36;
  int eyeRheightNext = 36;
  int eyeRx, eyeRy, eyeRxNext, eyeRyNext;
  byte eyeRborderRadiusCurrent = 8;
  byte eyeRborderRadiusNext = 8;
  
  int spaceBetweenCurrent = 10;
  int spaceBetweenNext = 10;
  
  // Mood control
  bool tired = 0;
  bool angry = 0;
  bool happy = 0;
  bool curious = 1;
  bool eyeL_open = 1;
  bool eyeR_open = 1;
  
  // Animations
  bool autoblinker = 1;
  int blinkInterval = 3;
  int blinkIntervalVariation = 2;
  unsigned long blinktimer = 0;
  
  bool idle = 1;
  int idleInterval = 2;
  int idleIntervalVariation = 2;
  unsigned long idleAnimationTimer = 0;
  
  bool confused = 0;
  unsigned long confusedAnimationTimer = 0;
  int confusedAnimationDuration = 500;
  bool confusedToggle = 1;
  
  bool laugh = 0;
  unsigned long laughAnimationTimer = 0;
  int laughAnimationDuration = 500;
  bool laughToggle = 1;
  
  // Eyelid variables
  byte eyelidsTiredHeight = 0;
  byte eyelidsTiredHeightNext = 0;
  byte eyelidsAngryHeight = 0;
  byte eyelidsAngryHeightNext = 0;
  byte eyelidsHappyBottomOffset = 0;
  byte eyelidsHappyBottomOffsetNext = 0;
  int eyeLheightOffset = 0;
  int eyeRheightOffset = 0;
  
  // Flicker
  bool hFlicker = 0;
  bool hFlickerAlternate = 0;
  byte hFlickerAmplitude = 2;
  
  bool vFlicker = 0;
  bool vFlickerAlternate = 0;
  byte vFlickerAmplitude = 10;
  
  // Love mode
  bool loveMode = 0;
  
  // Stone mode
  bool stoneMode = 0;
  
  // ━━━━━━━━━━━━━━━━━━━━
  // 🔧 PRIVATE HELPERS
  // ━━━━━━━━━━━━━━━━━━━━
  int getScreenConstraint_X();
  int getScreenConstraint_Y();
  void setMood(unsigned char mood);
  void setPosition(unsigned char position);
  void close();
  void open();
  void blink();
  void wink();
  void anim_confused();
  void anim_laugh();
  void drawEyes();
  void drawHeart(int x, int y, int size);
  void drawStone();
  
public:
  EyeAnimation(Adafruit_SSD1306* disp, BuzzerFX* buz);
  void begin();
  void update();
  
  // ━━━━━━━━━━━━━━━━━━━━
  // 🎭 EXPRESSION FUNCTIONS
  // ━━━━━━━━━━━━━━━━━━━━
  void expr_idle();
  void expr_happy();
  void expr_laugh();
  void expr_sad();
  void expr_angry();
  void expr_thinking();
  void expr_cool();
  void expr_shock();
  void expr_confused();
  void expr_clown();
  void expr_stone();
  void expr_normal();
  void expr_sleepy();
  void expr_love();
  void expr_wink();
  void expr_dizzy();
  void expr_bored();
};

#endif
