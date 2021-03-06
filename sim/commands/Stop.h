#pragma once

#include <ignition/math.hh>
#include <common/commanduino/CommanDuino.h>
#include <sim/lib/SimMouse.h>

class Stop : public Command {
public:
  Stop();

  Stop(unsigned long stop_time);

  void initialize();

  void execute();

  bool isFinished();

  void end();

private:
  SimMouse *mouse;
  double initial_velocity;
  unsigned long stop_time_ms = 500;
};

