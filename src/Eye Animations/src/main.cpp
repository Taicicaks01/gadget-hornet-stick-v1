// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// 🤖 EXPRESSIVE ROBOT EYE + SOUND SYSTEM
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// Using FluxGarage RoboEyes (smooth & cute!)
// + Custom BuzzerFX sound system
// Platform: Wemos D1 Mini (ESP8266)
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "BuzzerFX.h"
#include "EyeAnimation.h"
#include "seeRam.h"

// ━━━━━━━━━━━━━━━━━━━━
// 🔧 HARDWARE CONFIG
// ━━━━━━━━━━━━━━━━━━━━
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define BUZZER_PIN D5
#define BUTTON_PIN D6

// ━━━━━━━━━━━━━━━━━━━━
// 🎯 OBJECTS
// ━━━━━━━━━━━━━━━━━━━━
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
BuzzerFX buzzer(BUZZER_PIN);
EyeAnimation eyes(&display, &buzzer);
SeeRam ramMonitor;

// ━━━━━━━━━━━━━━━━━━━━
// 🎮 BUTTON CONTROL
// ━━━━━━━━━━━━━━━━━━━━
bool lastButtonState = HIGH;
bool buttonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
uint8_t currentExpressionIdx = 0;

// ━━━━━━━━━━━━━━━━━━━━
// 🎬 DEMO SEQUENCE
// ━━━━━━━━━━━━━━━━━━━━
struct DemoExpr {
  void (*func)();
  const char* name;
  uint16_t duration;
};

DemoExpr demoSeq[] = {
  {[](){ eyes.expr_idle(); },       "😐 Idle",         2000},
  {[](){ eyes.expr_happy(); },      "😊 Happy",        2000},
  {[](){ eyes.expr_laugh(); },      "😂 Laugh",        2000},
  {[](){ eyes.expr_thinking(); },   "🤔 Thinking",     2500},
  {[](){ eyes.expr_cool(); },       "😎 Cool",         2000},
  {[](){ eyes.expr_sleepy(); },     "😴 Sleepy",       2000},
  {[](){ eyes.expr_love(); },       "😍 Love",         2000},
  {[](){ eyes.expr_wink(); },       "😉 Wink",         1500},
  {[](){ eyes.expr_shock(); },      "😱 Shock",        1500},
  {[](){ eyes.expr_angry(); },      "😡 Angry",        2000},
  {[](){ eyes.expr_sad(); },        "😢 Sad",          2000},
  {[](){ eyes.expr_confused(); },   "😕 Confused",     2000},
  {[](){ eyes.expr_dizzy(); },      "😵 Dizzy",        2000},
  {[](){ eyes.expr_clown(); },      "🤡 Clown",        2000},
  {[](){ eyes.expr_bored(); },      "😑 Bored",        2500},
  {[](){ eyes.expr_stone(); },      "🗿 Stone Mode",   3000}
};

const uint8_t DEMO_COUNT = sizeof(demoSeq) / sizeof(DemoExpr);
uint8_t demoIdx = 0;
unsigned long demoTimer = 0;
bool autoMode = false;  // Start in manual button mode

// ━━━━━━━━━━━━━━━━━━━━
// 🚀 SETUP
// ━━━━━━━━━━━━━━━━━━━━
void setup() {
  Serial.begin(115200);
  delay(100);
  
  Serial.println(F("\n\n╔═══════════════════════════════════════════╗"));
  Serial.println(F("║  🤖 EXPRESSIVE ROBOT EYE SYSTEM v3.0     ║"));
  Serial.println(F("║  🎨 FluxGarage RoboEyes + BuzzerFX       ║"));
  Serial.println(F("║  ✨ Smooth & Cute Animations + Sound     ║"));
  Serial.println(F("╚═══════════════════════════════════════════╝\n"));

  // RAM/ROM status
  ramMonitor.logBoot();
  
  // Initialize button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("❌ SSD1306 failed!"));
    while (1);
  }
  
  display.clearDisplay();
  display.display();

  // Show RAM/ROM percentages on OLED briefly
  ramMonitor.drawBootStats(&display);
  delay(10000);
  
  // Initialize systems
  buzzer.begin();
  eyes.begin();
  
  // Start with idle
  eyes.expr_idle();
  demoTimer = millis();
  
  Serial.println(F("✅ System ready!"));
  Serial.println(F("🎮 Button at D6 - Press to cycle expressions"));
  Serial.println(F("📺 Send 'a' via serial for auto-demo mode\n"));
}

// ━━━━━━━━━━━━━━━━━━━━
// 🔄 MAIN LOOP
// ━━━━━━━━━━━━━━━━━━━━
void loop() {
  unsigned long currentTime = millis();
  
  // Update animation (non-blocking)
  eyes.update();
  
  // Update buzzer (non-blocking)
  buzzer.update();
  
  // ━━━━━━━━━━━━━━━━━━━━
  // 🎮 BUTTON CONTROL
  // ━━━━━━━━━━━━━━━━━━━━
  bool buttonReading = digitalRead(BUTTON_PIN);
  
  // Detect button state change (for debouncing)
  if (buttonReading != lastButtonState) {
    lastDebounceTime = currentTime;
    
    // Debug output
    Serial.print(F("🔘 Button state: "));
    Serial.println(buttonReading == LOW ? F("PRESSED") : F("RELEASED"));
  }
  
  // Update last button state
  lastButtonState = buttonReading;
  
  // Check if debounce time has passed and state is stable
  if ((currentTime - lastDebounceTime) > debounceDelay) {
    // If button state has changed from the stable state
    if (buttonReading != buttonState) {
      buttonState = buttonReading;
      
      // Button was just pressed (stable LOW state after debounce)
      if (buttonState == LOW) {
        // Cycle to next expression
        currentExpressionIdx = (currentExpressionIdx + 1) % DEMO_COUNT;
        
        Serial.print(F("🎮 Button Press: "));
        Serial.println(demoSeq[currentExpressionIdx].name);
        
        demoSeq[currentExpressionIdx].func();
        autoMode = false;  // Exit auto mode when button pressed
      }
    }
  }
  
  // ━━━━━━━━━━━━━━━━━━━━
  // 📺 SERIAL CONTROL
  // ━━━━━━━━━━━━━━━━━━━━
  if (Serial.available()) {
    char cmd = Serial.read();
    
    if (cmd == 'a' || cmd == 'A') {
      autoMode = !autoMode;
      Serial.print(F("🔄 Auto-demo mode: "));
      Serial.println(autoMode ? F("ON") : F("OFF"));
      demoTimer = currentTime;
    }
    else if (cmd >= '0' && cmd <= '9') {
      int idx = cmd - '0';
      if (idx < DEMO_COUNT) {
        currentExpressionIdx = idx;
        Serial.print(F("⌨️  Serial: "));
        Serial.println(demoSeq[idx].name);
        demoSeq[idx].func();
        autoMode = false;
      }
    }
  }
  
  // ━━━━━━━━━━━━━━━━━━━━
  // 🎬 AUTO-DEMO SEQUENCE
  // ━━━━━━━━━━━━━━━━━━━━
  if (autoMode) {
    if (currentTime - demoTimer >= demoSeq[demoIdx].duration) {
      demoIdx = (demoIdx + 1) % DEMO_COUNT;
      
      Serial.print(F("🎭 Auto: "));
      Serial.println(demoSeq[demoIdx].name);
      
      demoSeq[demoIdx].func();
      demoTimer = currentTime;
    }
  }
}
