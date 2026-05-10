#include "BuzzerFX.h"

// Pattern definitions (frequency in Hz, duration in ms)

const BuzzerNote BuzzerFX::PATTERN_TICK[] = {
  {1200, 20}
};

const BuzzerNote BuzzerFX::PATTERN_HAPPY[] = {
  {800, 100},
  {0, 30},
  {1000, 100},
  {0, 30},
  {1200, 150}
};

const BuzzerNote BuzzerFX::PATTERN_LAUGH[] = {
  {900, 80},
  {0, 30},
  {950, 80},
  {0, 30},
  {1000, 80},
  {0, 30},
  {1050, 100}
};

const BuzzerNote BuzzerFX::PATTERN_SAD[] = {
  {600, 150},
  {0, 50},
  {500, 200},
  {0, 50},
  {400, 300}
};

const BuzzerNote BuzzerFX::PATTERN_ANGRY[] = {
  {1400, 60},
  {0, 20},
  {1500, 60},
  {0, 20},
  {1400, 60},
  {0, 20},
  {1600, 100}
};

const BuzzerNote BuzzerFX::PATTERN_THINKING[] = {
  {700, 40},
  {0, 200},
  {720, 40},
  {0, 200},
  {740, 40}
};

const BuzzerNote BuzzerFX::PATTERN_COOL[] = {
  {300, 400}
};

const BuzzerNote BuzzerFX::PATTERN_SHOCK[] = {
  {2000, 50},
  {0, 30},
  {1800, 50},
  {0, 30},
  {2200, 70}
};

const BuzzerNote BuzzerFX::PATTERN_CLOWN[] = {
  {800, 80},
  {0, 40},
  {1200, 80},
  {0, 40},
  {700, 80},
  {0, 40},
  {1400, 100}
};

const BuzzerNote BuzzerFX::PATTERN_SCAN[] = {
  {1000, 50},
  {0, 100},
  {1100, 50},
  {0, 100},
  {1200, 50}
};

const BuzzerNote BuzzerFX::PATTERN_POWER[] = {
  {1200, 100},
  {1400, 100},
  {1600, 100},
  {1800, 100},
  {2000, 150}
};

BuzzerFX::BuzzerFX(uint8_t buzzerPin) {
  pin = buzzerPin;
  playing = false;
  enabled = true;
  currentPattern = nullptr;
  patternLength = 0;
  currentNote = 0;
  noteStartTime = 0;
}

void BuzzerFX::begin() {
  pinMode(pin, OUTPUT);
  stopNote();
}

void BuzzerFX::update() {
  if (!playing || !enabled || currentPattern == nullptr) {
    return;
  }
  
  unsigned long currentTime = millis();
  
  // Check if current note finished
  if (currentTime - noteStartTime >= currentPattern[currentNote].duration) {
    currentNote++;
    
    // Check if pattern finished
    if (currentNote >= patternLength) {
      stop();
      return;
    }
    
    // Play next note
    noteStartTime = currentTime;
    uint16_t freq = currentPattern[currentNote].frequency;
    
    if (freq > 0) {
      playNote(freq);
    } else {
      stopNote();
    }
  }
}

void BuzzerFX::play(SoundPattern pattern) {
  if (!enabled) return;
  
  stop();  // Stop any current sound
  
  switch (pattern) {
    case SOUND_TICK:
      playPattern(PATTERN_TICK, sizeof(PATTERN_TICK) / sizeof(BuzzerNote));
      break;
    case SOUND_HAPPY:
      playPattern(PATTERN_HAPPY, sizeof(PATTERN_HAPPY) / sizeof(BuzzerNote));
      break;
    case SOUND_LAUGH:
      playPattern(PATTERN_LAUGH, sizeof(PATTERN_LAUGH) / sizeof(BuzzerNote));
      break;
    case SOUND_SAD:
      playPattern(PATTERN_SAD, sizeof(PATTERN_SAD) / sizeof(BuzzerNote));
      break;
    case SOUND_ANGRY:
      playPattern(PATTERN_ANGRY, sizeof(PATTERN_ANGRY) / sizeof(BuzzerNote));
      break;
    case SOUND_THINKING:
      playPattern(PATTERN_THINKING, sizeof(PATTERN_THINKING) / sizeof(BuzzerNote));
      break;
    case SOUND_COOL:
      playPattern(PATTERN_COOL, sizeof(PATTERN_COOL) / sizeof(BuzzerNote));
      break;
    case SOUND_SHOCK:
      playPattern(PATTERN_SHOCK, sizeof(PATTERN_SHOCK) / sizeof(BuzzerNote));
      break;
    case SOUND_CLOWN:
      playPattern(PATTERN_CLOWN, sizeof(PATTERN_CLOWN) / sizeof(BuzzerNote));
      break;
    case SOUND_SCAN:
      playPattern(PATTERN_SCAN, sizeof(PATTERN_SCAN) / sizeof(BuzzerNote));
      break;
    case SOUND_POWER:
      playPattern(PATTERN_POWER, sizeof(PATTERN_POWER) / sizeof(BuzzerNote));
      break;
    default:
      break;
  }
}

void BuzzerFX::stop() {
  playing = false;
  currentPattern = nullptr;
  patternLength = 0;
  currentNote = 0;
  stopNote();
}

void BuzzerFX::mute() {
  enabled = false;
  stop();
}

void BuzzerFX::unmute() {
  enabled = true;
}

bool BuzzerFX::isPlaying() {
  return playing;
}

void BuzzerFX::playPattern(const BuzzerNote* pattern, uint8_t length) {
  if (length == 0 || pattern == nullptr) return;
  
  currentPattern = pattern;
  patternLength = length;
  currentNote = 0;
  noteStartTime = millis();
  playing = true;
  
  // Play first note
  uint16_t freq = pattern[0].frequency;
  if (freq > 0) {
    playNote(freq);
  } else {
    stopNote();
  }
}

void BuzzerFX::playNote(uint16_t frequency) {
  tone(pin, frequency);
}

void BuzzerFX::stopNote() {
  noTone(pin);
}
