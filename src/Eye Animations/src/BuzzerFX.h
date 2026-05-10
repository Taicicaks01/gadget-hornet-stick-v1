#ifndef BUZZER_FX_H
#define BUZZER_FX_H

#include <Arduino.h>

// Buzzer pattern structure
struct BuzzerNote {
  uint16_t frequency;  // Hz (0 = silence)
  uint16_t duration;   // milliseconds
};

// Predefined sound patterns
enum SoundPattern {
  SOUND_NONE,
  SOUND_TICK,           // Subtle tick
  SOUND_HAPPY,          // Ascending beep
  SOUND_LAUGH,          // Rapid beeps
  SOUND_SAD,            // Descending tone
  SOUND_ANGRY,          // Harsh beeps
  SOUND_THINKING,       // Soft ticks
  SOUND_COOL,           // Low slow beep
  SOUND_SHOCK,          // Glitch beep
  SOUND_CLOWN,          // Silly beeps
  SOUND_SCAN,           // Scan ticks
  SOUND_POWER           // Energetic buzz
};

class BuzzerFX {
private:
  uint8_t pin;
  
  // Current pattern state
  const BuzzerNote* currentPattern;
  uint8_t patternLength;
  uint8_t currentNote;
  unsigned long noteStartTime;
  bool playing;
  bool enabled;  // Global mute
  
  // Pattern definitions
  static const BuzzerNote PATTERN_TICK[];
  static const BuzzerNote PATTERN_HAPPY[];
  static const BuzzerNote PATTERN_LAUGH[];
  static const BuzzerNote PATTERN_SAD[];
  static const BuzzerNote PATTERN_ANGRY[];
  static const BuzzerNote PATTERN_THINKING[];
  static const BuzzerNote PATTERN_COOL[];
  static const BuzzerNote PATTERN_SHOCK[];
  static const BuzzerNote PATTERN_CLOWN[];
  static const BuzzerNote PATTERN_SCAN[];
  static const BuzzerNote PATTERN_POWER[];
  
public:
  BuzzerFX(uint8_t buzzerPin);
  
  void begin();
  void update();  // Call in loop()
  
  void play(SoundPattern pattern);
  void stop();
  void mute();
  void unmute();
  bool isPlaying();
  
private:
  void playPattern(const BuzzerNote* pattern, uint8_t length);
  void playNote(uint16_t frequency);
  void stopNote();
};

#endif
