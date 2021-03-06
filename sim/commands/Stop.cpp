#include "Stop.h"

Stop::Stop() : Command("STOP"), mouse(SimMouse::inst()), stop_time_ms(0) {}

Stop::Stop(unsigned long stop_time) : mouse(SimMouse::inst()), stop_time_ms(stop_time) {}

void Stop::initialize() {
  setTimeout(stop_time_ms);
}

void Stop::execute() {
  mouse->setSpeedCps(0.0, 0.0);
}

bool Stop::isFinished() {
  return isTimedOut();
}

void Stop::end() {
}

