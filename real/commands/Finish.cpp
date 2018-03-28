#include "Finish.h"

Finish::Finish(AbstractMaze *maze) : Command("finish"), maze(maze), mouse(RealMouse::inst()) {
}

void Finish::initialize() {
  mouse->setSpeedCps(0, 0);
  setTimeout(2000);
  t = getTime();
  pin_id = RealMouse::LED_1;
  on = true;
}

void Finish::execute() {
  if (getTime() - t > BLINK_TIME) {

    on = !on;
    t = getTime();

    if (on) {
      digitalWrite(pin_id, 1);
    } else {
      digitalWrite(pin_id, 0);
      pin_id++;
      if (pin_id > RealMouse::LED_1) {
        pin_id = RealMouse::LED_2;
      }
    }
  }
}

bool Finish::isFinished() {
  return isTimedOut();
}
