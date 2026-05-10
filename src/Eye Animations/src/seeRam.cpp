// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// 💾 RAM/ROM MONITOR IMPLEMENTATION
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

#include "seeRam.h"
#include <ESP.h>
#include <Adafruit_SSD1306.h>

void SeeRam::logBoot() {
  Serial.println(F("\n[RAM/ROM CHECK]"));
  Serial.print(F("Free Heap: ")); Serial.print(ESP.getFreeHeap()); Serial.println(F(" bytes"));
  Serial.print(F("Max Contiguous Heap: ")); Serial.print(ESP.getMaxFreeBlockSize()); Serial.println(F(" bytes"));
  Serial.print(F("Sketch Size: ")); Serial.print(ESP.getSketchSize()); Serial.println(F(" bytes"));
  Serial.print(F("Free Sketch Space: ")); Serial.print(ESP.getFreeSketchSpace()); Serial.println(F(" bytes"));
  Serial.print(F("Flash Chip Size: ")); Serial.print(ESP.getFlashChipSize()); Serial.println(F(" bytes"));
  Serial.println();
}

void SeeRam::drawBootStats(Adafruit_SSD1306* display) {
  if (!display) return;

  // Gather stats
  uint32_t freeHeap = ESP.getFreeHeap();
  uint32_t maxBlock = ESP.getMaxFreeBlockSize();
  uint32_t flashSize = ESP.getFlashChipSize();
  uint32_t sketchSize = ESP.getSketchSize();
  uint32_t freeSketch = ESP.getFreeSketchSpace();
  int heapFrag = ESP.getHeapFragmentation(); // percent

  // Derived values (fixed heap total for ESP8266 Wemos D1 Mini)
  const uint32_t heapTotal = 81920UL; // bytes
  uint32_t heapUsed = (freeHeap <= heapTotal) ? (heapTotal - freeHeap) : 0;
  uint32_t flashUsed = sketchSize;

  auto pct10 = [](uint32_t used, uint32_t total) -> uint16_t {
    if (total == 0) return 0;
    // tenths of a percent to avoid float
    return (uint16_t)((used * 1000UL) / total);
  };

  uint16_t heapPct10 = pct10(heapUsed, heapTotal);   // e.g. 362 -> 36.2%
  uint16_t flashPct10 = pct10(flashUsed, flashSize); // e.g. 281 -> 28.1%

  auto drawBar = [&](int16_t x, int16_t y, uint16_t pctTenths) {
    if (pctTenths > 1000) pctTenths = 1000;
    const int16_t w = 96;
    const int16_t h = 10;
    display->drawRect(x, y, w, h, WHITE);
    int16_t fill = (pctTenths * (w - 4)) / 1000;
    display->fillRect(x + 2, y + 2, fill, h - 4, WHITE);
  };

  // Render compact view on OLED
  display->clearDisplay();
  display->setTextColor(WHITE);
  display->setTextSize(1);

  display->setCursor(0, 0);
  display->println(F("Resource Monitor"));

  // RAM row (percent + bar only)
  display->setCursor(0, 14);
  display->print(F("RAM "));
  display->print(heapPct10 / 10); display->print(F(".")); display->print(heapPct10 % 10); display->println(F("%"));
  drawBar(0, 24, heapPct10);

  // Flash row (percent + bar only)
  display->setCursor(0, 44);
  display->print(F("Flash "));
  display->print(flashPct10 / 10); display->print(F(".")); display->print(flashPct10 % 10); display->println(F("%"));
  drawBar(0, 54, flashPct10);

  display->display();
}
