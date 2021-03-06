#include <common/core/Mouse.h>
#include <real/commands/WaitForStart.h>
#include <common/KinematicController/VelocityProfile.h>

WaitForStart::WaitForStart()
    : CommandGroup("wait_calibrate"), mouse(RealMouse::inst()), speed(0) {}

void WaitForStart::initialize() {
  mouse->left_encoder.ResetPosition();
  mouse->right_encoder.ResetPosition();
  mouse->kinematic_controller.enabled = false;
}

void WaitForStart::execute() {
  CommandGroup::execute();

  // Set the max speed of the robot based on the left wheel
  double percent_speed =
      static_cast<double>(mouse->left_encoder.getUnsignedRotation()) / smartmouse::kc::TICKS_PER_REVOLUTION;
  speed = percent_speed * smartmouse::kc::MAX_HARDWARE_SPEED_MPS;
  auto right = mouse->right_encoder.getUnsignedRotation();
  if (right < smartmouse::kc::TICKS_PER_REVOLUTION / 3) {
    GlobalProgramSettings.dead_reckoning = false;
    smartmouse::kc::WALL_SMASH = false;
  }
  else if (right < smartmouse::kc::TICKS_PER_REVOLUTION * 2 / 3) {
    GlobalProgramSettings.dead_reckoning = true;
    smartmouse::kc::WALL_SMASH = false;
  }
  else {
    GlobalProgramSettings.dead_reckoning = false;
    smartmouse::kc::WALL_SMASH = true;
  }


  digitalWrite(RealMouse::LED_1, static_cast<uint8_t>(GlobalProgramSettings.dead_reckoning));
  digitalWrite(RealMouse::LED_2, static_cast<uint8_t>(smartmouse::kc::WALL_SMASH));

  constexpr uint8_t NUM_LEDS = 6;
  uint8_t light_up_until_led_index = static_cast<uint8_t >(percent_speed * NUM_LEDS);
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    if (i < light_up_until_led_index) {
      digitalWrite(RealMouse::LED_7 - i, 1);
    } else {
      digitalWrite(RealMouse::LED_7 - i, 0);
    }
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
  // FIXME:
  smartmouse::kc::MAX_SPEED_MPS = max(speed, 0.15);
//  GlobalProgramSettings.dead_reckoning = false;
//  smartmouse::kc::MAX_SPEED_MPS = 0.15;
  smartmouse::kc::MAX_SPEED_CUPS = smartmouse::maze::toCellUnits(smartmouse::kc::MAX_SPEED_MPS);
  for (uint8_t i = 0; i < 7; i++) {
    digitalWrite(RealMouse::LED_7 - i, 0);
  }
  mouse->resetToStartPose();
  mouse->kinematic_controller.enabled = true;
}
