#ifndef STRIKES_H
#define STRIKES_H

#include <Arduino.h>
#include <main_module.h>

namespace Strikes {
const int DELAY = 2;
const int BLINK_DELAY = 500;
const int MAX_STRIKES_DISPLAYED = 3;

const int SEGMENTS = 8;
const int SEGMENT_PINS[SEGMENTS] = {18, 5, 13, 14, 16, 4, 12, 27};
const uint8_t STRIKE_SEGMENTS = 0b00010110;

const int STRIKE_DIGITS = 4;
const int STRIKE_DIGIT_PINS[STRIKE_DIGITS] = {17, 15, 2, 19};

bool selected = false;
unsigned long lastSelected = 0;

void setup() {
  for (int i = 0; i < STRIKE_DIGITS; i++) {
    pinMode(STRIKE_DIGIT_PINS[i], OUTPUT);
    digitalWrite(STRIKE_DIGIT_PINS[i], LOW);
  }
}

void off() {
  for (int i = 0; i < STRIKE_DIGITS; i++) {
    digitalWrite(STRIKE_DIGIT_PINS[i], LOW);
  }
}

void setSelected(bool s) {
  selected = s;
  if (selected)
    lastSelected = millis();
}

void update() {
  unsigned long now = millis();
  int index = (now / DELAY) % STRIKE_DIGITS;
  int digit_index = index / 2;
  int last_index = (index - 1 + STRIKE_DIGITS) % STRIKE_DIGITS;
  digitalWrite(STRIKE_DIGIT_PINS[last_index], LOW);
  uint8_t segments = 0;
  int strikes =
      MAX_STRIKES_DISPLAYED - MainModule::maxStrikes() + MainModule::strikes();
  if (strikes > digit_index)
    segments = STRIKE_SEGMENTS;
  if (selected) {
    bool isOn = ((now - lastSelected) / BLINK_DELAY) % 2 == 0;
    if (!isOn)
      segments = 0b00000000;
  }
  for (int i = 0; i < SEGMENTS; i++) {
    digitalWrite(SEGMENT_PINS[i], ((~segments) >> (SEGMENTS - i - 1)) & 1);
  }
  digitalWrite(STRIKE_DIGIT_PINS[index], HIGH);
}
}; // namespace Strikes

#endif // STRIKES_H