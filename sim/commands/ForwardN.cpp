#include "ForwardN.h"

ForwardN::ForwardN(unsigned int n) : Command("Forward"), mouse(SimMouse::inst()), n(n) {}


void ForwardN::initialize() {
  start = mouse->getGlobalPose();
  mouse->kinematic_controller.enable_sensor_pose_estimate = true;
  const double goal_disp = KinematicController::dispToNthEdge(*mouse, n);
  const double v0 = mouse->kinematic_controller.getCurrentForwardSpeedCUPS();
  const double vf = smartmouse::kc::END_SPEED_CUPS;
  drive_straight_state = new smartmouse::kc::DriveStraightState(start, goal_disp, v0, vf);
  // TODO: eventually use this instead
  // mouse->kinematic_controller.plan_traj(start, KinematicController::poseOfNthEdge(mouse, n));
}

void ForwardN::execute() {
  double l, r;
  std::tie(l, r) = drive_straight_state->compute_wheel_velocities(*mouse);
  mouse->setSpeedCps(l, r);
}

bool ForwardN::isFinished() {
  return drive_straight_state->dispError() <= 0;
}

void ForwardN::end() {
  mouse->kinematic_controller.enable_sensor_pose_estimate = false;
  mouse->pauseSim();
}

