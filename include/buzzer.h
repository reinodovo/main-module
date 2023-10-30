#ifndef BUZZER_H
#define BUZZER_H

#include <main_module.h>
#include <queue>

namespace Buzzer {
enum class BuzzerFrequency {
  Solve,
  Strike = 233,
  Second = 3728,
  MidSecond = 4186,
};

struct BuzzerEvent {
  unsigned long time;
  unsigned long duration;
  BuzzerFrequency frequency;
};

const unsigned long STRIKE_DURATION = 400;
const unsigned long FAIL_DURATION = 2000;

unsigned long BUZZER_DELAYS[MainModule::SPEED_STAGES] = {700, 600, 400, 150};

std::queue<BuzzerEvent> buzzerQueue;
int buzzerPin;

void start(int pin) {
  buzzerPin = pin;
  buzzerQueue.push({millis(), 70, BuzzerFrequency::Second});
}

void clear() {
  while (!buzzerQueue.empty())
    buzzerQueue.pop();
}

void playStrike() {
  clear();
  buzzerQueue.push({millis(), STRIKE_DURATION, BuzzerFrequency::Strike});
}

void playFail() {
  clear();
  buzzerQueue.push({millis(), FAIL_DURATION, BuzzerFrequency::Strike});
  buzzerQueue.push({millis() + FAIL_DURATION, 0, BuzzerFrequency::Strike});
}

void update() {
  if (buzzerQueue.empty())
    return;
  if (MainModule::solved())
    return;
  BuzzerEvent event = buzzerQueue.front();
  unsigned long currentTime = millis();
  if (event.time > currentTime)
    return;
  buzzerQueue.pop();
  noTone(buzzerPin);
  if (event.duration)
    tone(buzzerPin, (int)event.frequency, event.duration);
  if (MainModule::failed())
    return;
  if (event.frequency == BuzzerFrequency::MidSecond)
    buzzerQueue.push({currentTime + MainModule::timeToNextSecond(), 70,
                      BuzzerFrequency::Second});
  else if (event.frequency == BuzzerFrequency::Second)
    buzzerQueue.push({currentTime + BUZZER_DELAYS[MainModule::speed()], 100,
                      BuzzerFrequency::MidSecond});
  else if (event.frequency == BuzzerFrequency::Strike)
    buzzerQueue.push(
        {currentTime + STRIKE_DURATION, 0, BuzzerFrequency::MidSecond});
}
} // namespace Buzzer

#endif // BUZZER_H