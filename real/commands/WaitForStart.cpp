#include <common/core/Mouse.h>
#include "WaitForStart.h"
#include "Calibrate.h"

bool WaitForStart::calibrated = false;

WaitForStart::WaitForStart()
    : CommandGroup("wait_calibrate"), mouse(RealMouse::inst()), speed(0), init_ticks_left(0), init_ticks_right(0) {
  mouse->kinematic_controller.enabled = false;
  if (!calibrated) {
    addSequential(new Calibrate());
    calibrated = true;
  }
}

void WaitForStart::initialize() {
  init_ticks_left = mouse->left_encoder.getRotation();
  init_ticks_right = mouse->right_encoder.getRotation();
}

void WaitForStart::execute() {
  CommandGroup::execute();

  // Set the max speed of the robot based on the right wheel
  double percent_speed = fmod(mouse->left_encoder.getRotation() - init_ticks_left, 1024) / 1024;
  speed = percent_speed * smartmouse::kc::MAX_HARDWARE_SPEED_MPS;

  uint8_t light_up_until_led_index = static_cast<uint8_t >(percent_speed * 7);
  for (uint8_t i = 0; i < 7; i++) {
    if (i < light_up_until_led_index) {
      digitalWrite(RealMouse::LED_7 - i, 1);
    } else {
      digitalWrite(RealMouse::LED_7 - i, 0);
    }
  }

  // Set arc turn on or off based on left wheel
  smartmouse::kc::ARC_TURN = fmod(fabs(mouse->right_encoder.getRotation() - init_ticks_right), 200) > 100;
  if (smartmouse::kc::ARC_TURN) {
    digitalWrite(RealMouse::LED_1, 1);
  } else {
    digitalWrite(RealMouse::LED_1, 0);
  }
}

bool WaitForStart::isFinished() {
  if (CommandGroup::isFinished()) {
    return !digitalRead(RealMouse::BUTTON_PIN);
  } else {
    return false;
  }
}

void WaitForStart::end() {
  smartmouse::kc::MAX_SPEED_MPS = max(speed, 0.1);
  smartmouse::kc::MAX_SPEED_CUPS = smartmouse::maze::toCellUnits(smartmouse::kc::MAX_SPEED_MPS);
  for (int i = 0; i < 7; i++) {
    digitalWrite(RealMouse::LED_7 - i, 0);
  }
  mouse->resetToStartPose();
  mouse->kinematic_controller.enabled = true;
}
