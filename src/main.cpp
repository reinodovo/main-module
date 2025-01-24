#include <Arduino.h>
#include <EEPROM.h>

#include <main_module.h>
#include <utils/button.h>

#include <buzzer.h>
#include <display.h>
#include <strikes.h>
#include <timer.h>

struct Settings {
  unsigned long duration;
  int maxStrikes;
} settings;

const int START_COUNTDOWN_SECONDS = 3;

const int BUZZER_PIN = 21;

const int TIMER_DIGITS = 4;
const unsigned long DIGIT_DURATIONS[4] = {600000ul, 60000ul, 10000ul, 1000ul};
int digits[TIMER_DIGITS] = {0, 2, 0, 0};

const int SELECT_BUTTON_PIN = 34;
const int CHANGE_BUTTON_PIN = 35;
Button selectButton, changeButton;

void handleSelectButton(ButtonState, ButtonState);
void handleChangeButton(ButtonState, ButtonState);
void updateSettings();
void recallSettings();

bool can_start;

void setup() {
  if (!MainModule::setup())
    ESP.restart();

  EEPROM.begin(sizeof(Settings));
  recallSettings();

  selectButton = Button(SELECT_BUTTON_PIN);
  selectButton.onStateChange = handleSelectButton;
  changeButton = Button(CHANGE_BUTTON_PIN);
  changeButton.onStateChange = handleChangeButton;

  Timer::setup();
  Strikes::setup();

  MainModule::onFailed = []() { Buzzer::playFail(); };
  MainModule::onStrike = [](int strikes) {
    if (MainModule::failed())
      return;
    Buzzer::playStrike();
  };

  can_start = true;
}

void recallSettings() {
  EEPROM.get(0, settings);
  unsigned long duration = settings.duration;
  for (int i = 0; i < TIMER_DIGITS; i++) {
    digits[i] = (duration / DIGIT_DURATIONS[i]);
    duration %= DIGIT_DURATIONS[i];
  }
  updateSettings();
}

void updateSettings() {
  settings.duration = 0;
  for (int i = 0; i < TIMER_DIGITS; i++)
    settings.duration += digits[i] * DIGIT_DURATIONS[i];
  EEPROM.put(0, settings);
  EEPROM.commit();
  MainModule::setDuration(settings.duration);
  MainModule::setMaxStrikes(settings.maxStrikes);
}

bool buttonsEnabled() {
  bool started = MainModule::started() || MainModule::starting();
  bool finished = MainModule::solved() || MainModule::failed();
  return !started || finished;
}

bool bothButtonsHeld() {
  return selectButton.state() == ButtonState::Held &&
         changeButton.state() == ButtonState::Held;
}

void reset() {
  can_start = false;
  MainModule::reset();
  Timer::showingCode = false;
  Timer::selectedDigit = 0;
  Strikes::setSelected(false);
  recallSettings();
}

void start() {
  if (!can_start)
    return;
  Timer::showingCode = false;
  Timer::selectedDigit = -1;
  Strikes::setSelected(false);
  MainModule::startAfter(START_COUNTDOWN_SECONDS);
  Buzzer::start(BUZZER_PIN);
}

void handleSelectButton(ButtonState state, ButtonState last) {
  if (!buttonsEnabled())
    return;
  if (state == ButtonState::Held)
    Timer::showingCode = true;
  if (state == ButtonState::Released)
    Timer::showingCode = false;
  if (state == ButtonState::Released && last == ButtonState::Pressed)
    Timer::selectedDigit = (Timer::selectedDigit + 1) % (TIMER_DIGITS + 1);
  Strikes::setSelected(Timer::selectedDigit == TIMER_DIGITS);
}

void handleChangeButton(ButtonState state, ButtonState last) {
  if (!buttonsEnabled())
    return;
  if (state != ButtonState::Released || last != ButtonState::Pressed)
    return;
  if (Timer::selectedDigit == TIMER_DIGITS) {
    settings.maxStrikes = (settings.maxStrikes % 3) + 1;
    Strikes::setSelected(true);
  } else {
    if (Timer::selectedDigit == 2)
      digits[Timer::selectedDigit] = (digits[Timer::selectedDigit] + 1) % 6;
    else
      digits[Timer::selectedDigit] = (digits[Timer::selectedDigit] + 1) % 10;
    Timer::lastChange = millis();
  }
  updateSettings();
}

void loop() {
  selectButton.update();
  changeButton.update();
  if (bothButtonsHeld() && buttonsEnabled()) {
    if (MainModule::solved() || MainModule::failed())
      reset();
    else
      start();
  } else
    can_start = true;
  Buzzer::update();
  MainModule::update();
  Display::update();
}