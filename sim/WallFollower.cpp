#ifdef SIM

#include "WallFollower.h"

WallFollower::WallFollower() : disp(0.0), goalDisp(AbstractMaze::UNIT_DIST), dispError(goalDisp), lastLeftWallError(0),
                               lastRightWallError(0) {}

WallFollower::WallFollower(double goalDisp) : disp(0.0), goalDisp(goalDisp), dispError(goalDisp), lastLeftWallError(0),
                                              lastRightWallError(0) {}

std::pair<double, double>
WallFollower::compute_wheel_velocities(SimMouse *mouse, Pose start_pose, SimMouse::RangeData range_data) {
  Pose current_pose = mouse->getEstimatedPose();
  disp = forwardDisplacement(mouse->getDir(), start_pose, current_pose);

  double currentYaw = current_pose.yaw;
  angleError = yawDiff(toYaw(mouse->getDir()), currentYaw);
  dToWallLeft = sin(SimMouse::ANALOG_ANGLE + angleError) * range_data.left_analog
                + sin(angleError) * SimMouse::SIDE_ANALOG_X
                + cos(angleError) * SimMouse::SIDE_ANALOG_Y;
  dToWallRight = -(sin(-SimMouse::ANALOG_ANGLE + angleError) * range_data.right_analog
                   + sin(angleError) * SimMouse::SIDE_ANALOG_X
                   + cos(angleError) * -SimMouse::SIDE_ANALOG_Y);


  dispError = goalDisp - disp;
  double l = SimMouse::MAX_SPEED;
  double r = SimMouse::MAX_SPEED;

  l = l > SimMouse::MAX_SPEED ? SimMouse::MAX_SPEED : l;
  r = r > SimMouse::MAX_SPEED ? SimMouse::MAX_SPEED : r;

  l = l < SimMouse::MIN_SPEED ? SimMouse::MIN_SPEED : l;
  r = r < SimMouse::MIN_SPEED ? SimMouse::MIN_SPEED : r;

  double leftWallError = AbstractMaze::HALF_INNER_UNIT_DIST - dToWallLeft;
  double rightWallError = AbstractMaze::HALF_INNER_UNIT_DIST - dToWallRight;
  double dLeftWallError = leftWallError - lastLeftWallError;
  double dRightWallError = rightWallError - lastRightWallError;
  lastLeftWallError = leftWallError;
  lastRightWallError = rightWallError;

  if (dToWallLeft < SimMouse::WALL_DIST) {
    double correction = leftWallError * kPWall + dLeftWallError * kDWall;
    r -= correction;
  } else if (dToWallRight < SimMouse::WALL_DIST) {
    double correction = rightWallError * kPWall + dRightWallError * kDWall;
    l -= correction;
  } else if (AbstractMaze::HALF_INNER_UNIT_DIST < dToWallLeft && dToWallLeft < SimMouse::WALL_DIST) {
    double correction = leftWallError * kPWall + dLeftWallError * kDWall;
    l -= correction;
  } else if (AbstractMaze::HALF_INNER_UNIT_DIST < dToWallRight && dToWallRight < SimMouse::WALL_DIST) {
    double correction = rightWallError * kPWall + dRightWallError * kDWall;
    r -= correction;
  }

  return std::pair<double, double>(l, r);
}

double
WallFollower::forwardDisplacement(Direction dir, Pose start_pose, Pose end_pose) {
  switch (dir) {
    case Direction::N:
      return end_pose.y - start_pose.y;
    case Direction::E:
      return end_pose.x - start_pose.x;
    case Direction::S:
      return start_pose.y - end_pose.y;
    case Direction::W:
      return start_pose.x - end_pose.x;
  }
}

double WallFollower::yawDiff(double y1, double y2) {
  double diff = y2 - y1;
  if (diff > M_PI) return diff - M_PI * 2;
  if (diff < -M_PI) return diff + M_PI * 2;
  return diff;
}

double WallFollower::dispToEdge(SimMouse *mouse) {
  double row_offset = mouse->getRowOffsetToEdge();
  double col_offset = mouse->getColOffsetToEdge();
  Direction dir = mouse->getDir();

  switch (dir) {
    case Direction::N:
      return AbstractMaze::UNIT_DIST - row_offset;
    case Direction::S:
      return row_offset;
    case Direction::E:
      return AbstractMaze::UNIT_DIST - col_offset;
    case Direction::W:
      return col_offset;
  }
}

double WallFollower::dispToCenter(SimMouse *mouse) {
  // TODO: Should be HALF_UNIT_DIST but this is just a hack to prevent it from going too far
  // TODO: this can be removed when we do proper ArcTurn stuff
  double row_offset_to_center = AbstractMaze::HALF_UNIT_DIST - mouse->getRowOffsetToEdge();
  double col_offset_to_center = AbstractMaze::HALF_UNIT_DIST - mouse->getColOffsetToEdge();
  Direction dir = mouse->getDir();

  switch (dir) {
    case Direction::S:
      return AbstractMaze::UNIT_DIST - row_offset_to_center;
    case Direction::N:
      return row_offset_to_center;
    case Direction::W:
      return AbstractMaze::UNIT_DIST - col_offset_to_center;
    case Direction::E:
      return col_offset_to_center;
  }
}

#endif
