/**
 * Test the PID values by executing a fixed velocity profile
 */

#include <real/RealMouse.h>
#include <common/commanduino/Command.h>
#include <common/commanduino/Scheduler.h>
#include <real/ArduinoTimer.h>
#include <common/KinematicController/VelocityProfile.h>

ArduinoTimer timer;
Scheduler *scheduler;
RealMouse *mouse;
unsigned long last_t_us, last_blink_us;
bool done = false;
bool on = true;
smartmouse::kc::VelocityProfile *profile;
unsigned long t_0;

void setup() {
  delay(1000);
  analogWriteResolution(10);

  Command::setTimerImplementation(&timer);
  mouse = RealMouse::inst();
  mouse->setup();

  mouse->kinematic_controller.enable_sensor_pose_estimate = false;

  last_t_us = timer.programTimeUs();

  smartmouse::kc::MAX_SPEED_CUPS = 1;

  GlobalPose start(0, 0, 0);
  profile = new smartmouse::kc::VelocityProfile(start, {2.5, 0, 0});
  t_0 = millis();
}

void loop() {
  RealMouse::checkVoltage();

  unsigned long now_us = timer.programTimeUs();
  double dt_us = now_us - last_t_us;

  if (now_us - last_blink_us > 50000) {
    last_blink_us = now_us;
    digitalWrite(RealMouse::SYS_LED, static_cast<uint8_t>(on));
    on = !on;
  }

  // minimum period of main loop
  if (dt_us < 1500) {
    return;
  }

  auto dt_s = dt_us / 1e6;
  mouse->run(dt_s);

  // time since start of the motion in seconds
  // since this program only does this once it's just millis converted to seconds
  auto t_s = static_cast<double>(millis() - t_0) / 1000.0;
  double v = profile->compute_forward_velocity(t_s);
  mouse->setSpeedCps(v, v);

  static unsigned long idx = 0;
  ++idx;
  if (idx % 10 == 0 and v > 0) {
    print("%8.2f, %8.2f, % 8.2f, % 8.2f\r\n",
          mouse->kinematic_controller.left_motor.setpoint_rps,
          mouse->kinematic_controller.left_motor.velocity_rps,
          mouse->kinematic_controller.right_motor.setpoint_rps,
          mouse->kinematic_controller.right_motor.velocity_rps);
  }

  last_t_us = now_us;
}
