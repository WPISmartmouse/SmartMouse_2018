#pragma once

#include <ignition/math.hh>
#include <common/commanduino/CommanDuino.h>
#include <common/Direction.h>
#include <common/WallFollower.h>

#include "SimMouse.h"

class ArcTurn : public Command {
public:
  ArcTurn(Direction dir);

  void initialize();

  void execute();

  bool isFinished();

  void end();

private:
  SimMouse *mouse;
  Direction dir;
  bool left; //false means right
  double turn_effort;
  double goal_x, goal_y;
  int start_row, start_col;
  static const double kTurn;
};

