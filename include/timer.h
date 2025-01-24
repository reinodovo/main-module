#ifndef TIMER_H
#define TIMER_H

#include <main_module.h>

const uint8_t NUMBERS[10] = {
    0b11111100, 0b01100000, 0b11011010, 0b11110010, 0b01100110,
    0b10110110, 0b10111110, 0b11100000, 0b11111110, 0b11110110,
};

namespace Timer {
const int SEPARATOR_POSITION = 2;
const unsigned long DELAY = 2;
const unsigned long BLINK_DELAY = 500;

const int SEGMENTS = 8;
const int SEGMENT_PINS[SEGMENTS] = {18, 5, 13, 14, 16, 4, 12, 27};

const int TIMER_DIGITS = 4;
const int TIMER_DIGIT_PINS[TIMER_DIGITS] = {32, 33, 25, 26};

bool showingCode = false;
int selectedDigit = 0;
unsigned long lastChange = 0;

int get_value(int digit, const char *time) {
  if (showingCode) {
    int code = MainModule::code();
    for (int i = TIMER_DIGITS - 1; i > digit; i--) {
      code /= 10;
    }
    return code % 10;
  }
  if (digit >= SEPARATOR_POSITION)
    digit++;
  return time[digit] - '0';
}

bool has_dot(int digit, const char *time) {
  if (showingCode)
    return false;
  return digit == 1 || (digit == 2 && time[SEPARATOR_POSITION] == ':');
}

void setup() {
  for (int i = 0; i < TIMER_DIGITS; i++) {
    pinMode(TIMER_DIGIT_PINS[i], OUTPUT);
    digitalWrite(TIMER_DIGIT_PINS[i], LOW);
  }
  for (int i = 0; i < SEGMENTS; i++) {
    pinMode(SEGMENT_PINS[i], OUTPUT);
    digitalWrite(SEGMENT_PINS[i], HIGH);
  }
}

void off() {
  for (int i = 0; i < TIMER_DIGITS; i++) {
    digitalWrite(TIMER_DIGIT_PINS[i], LOW);
  }
}

void update() {
  unsigned long now = millis();
  int digit = (now / DELAY) % TIMER_DIGITS;
  int lastDigit = (digit - 1 + TIMER_DIGITS) % TIMER_DIGITS;
  digitalWrite(TIMER_DIGIT_PINS[lastDigit], LOW);
  char *time;
  if (MainModule::onStartCountdown())
    time = MainModule::timeStrToStart();
  else
    time = MainModule::timeStr(selectedDigit == -1);
  int value = get_value(digit, time);
  bool dot = has_dot(digit, time);
  delete time;
  uint8_t segments = (~(NUMBERS[value] | (0b00000001 * dot)));
  if (!showingCode && selectedDigit == digit) {
    bool isOn = ((now - lastChange) / BLINK_DELAY) % 2 == 0;
    if (!isOn)
      segments = 0b11111111;
  }
  for (int i = 0; i < SEGMENTS; i++) {
    digitalWrite(SEGMENT_PINS[i], (segments >> (SEGMENTS - i - 1)) & 1);
  }
  digitalWrite(TIMER_DIGIT_PINS[digit], HIGH);
}
} // namespace Timer

#endif // TIMER_H