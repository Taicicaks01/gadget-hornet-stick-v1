// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// 💾 RAM/ROM MONITOR
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#pragma once

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

class SeeRam {
public:
  void logBoot();
  void drawBootStats(Adafruit_SSD1306* display);
};
