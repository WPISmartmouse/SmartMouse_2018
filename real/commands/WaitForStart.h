#pragma once

#include <common/commanduino/CommanDuino.h>
#include "RealMouse.h"
//#include <list> // THIS MUST GO LAST (idk why...)

class WaitForStart : public Command {
public:
  WaitForStart();
  void initialize();
  void execute();
  bool isFinished();
  void end();

private:
  RealMouse *mouse;
};
