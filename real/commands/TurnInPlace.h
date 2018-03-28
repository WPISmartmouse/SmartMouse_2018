#pragma once
#include <common/commanduino/CommanDuino.h>
#include <common/core/Direction.h>

#include <real/RealMouse.h>

class TurnInPlace : public Command {
public:
  explicit TurnInPlace(Direction dir);

  void initialize();

  void execute();

  bool isFinished();

  void end();

private:
  double goal_yaw;
  double yaw_error;
  RealMouse *mouse;
  Direction dir;

  const double kP = 0.15;
};

