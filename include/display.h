#include <strikes.h>
#include <timer.h>

namespace Display {
const int DISPLAY_ACTIVE_TIME = 8;
void update() {
  unsigned long now = millis();
  int display_index = (now / DISPLAY_ACTIVE_TIME) % 2;
  if (display_index == 0) {
    Strikes::off();
    Timer::update();
  } else {
    Timer::off();
    Strikes::update();
  }
}
}; // namespace Display