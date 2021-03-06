#include <Arduino.h>
#include <real/ArduinoTimer.h>
#include <common/commanduino/CommanDuino.h>
#include <real/RealMouse.h>

#include <common/commands/SolveCommand.h>
#include <common/core/Flood.h>
#include <real/commands/WaitForStart.h>
#include <real/commands/Stop.h>
#include <real/commands/ForwardToCenter.h>
#include <real/commands/TurnInPlace.h>
#include <real/commands/Turn.h>
#include <real/commands/ForwardN.h>
#include <real/commands/Forward.h>
#include <real/commands/Backup.h>

ArduinoTimer timer;
Scheduler *scheduler;
RealMouse *mouse;
unsigned long last_t_us, last_blink_us;
bool done = false;
bool on = true;
bool paused = false;

class NavTestCommand : public CommandGroup {
 public:
  NavTestCommand() : CommandGroup("NavTestGroup") {
    addSequential(new WaitForStart());
    addSequential(new Stop(1000));
    addSequential(new ForwardN(2));
    addSequential(new Turn(Direction::S));
//    addSequential(new Turn(Direction::S));
//    addSequential(new Turn(Direction::W));
//    addSequential(new ForwardN(1));
//    addSequential(new Turn(Direction::E));
//    addSequential(new Turn(Direction::S));
//    addSequential(new Turn(Direction::W));
//    addSequential(new ForwardN(2));
    addSequential(new Stop(1000));
  }
};

void setup() {
  Command::setTimerImplementation(&timer);
  mouse = RealMouse::inst();
  mouse->setup();

  GlobalProgramSettings.quiet = false;

//  scheduler = new Scheduler(new NavTestCommand());
  scheduler = new Scheduler(new SolveCommand(new Flood(mouse)));

  last_t_us = timer.programTimeMs();
  last_blink_us = timer.programTimeMs();
}

void loop() {
  RealMouse::checkVoltage();

  unsigned long now_us = timer.programTimeUs();
  double dt_us = now_us - last_t_us;

  if (now_us - last_blink_us > 1000000) {
    last_blink_us = now_us;
    digitalWrite(RealMouse::SYS_LED, static_cast<uint8_t>(on));
    on = !on;
  }

  // minimum period of main loop
  if (dt_us < 1500) {
    return;
  }

  if (not paused and not done) {
    // one of these should be commented out
    done = scheduler->run();
  } else {
    mouse->setSpeedCps(0, 0);
    digitalWrite(RealMouse::SYS_LED, 1);
    digitalWrite(RealMouse::LED_2, 1);
    digitalWrite(RealMouse::LED_4, 1);
    digitalWrite(RealMouse::LED_6, 1);
  }

  auto dt_s = dt_us / 1e6;

  mouse->run(dt_s);

//  print_slow("%4.3f %4.3f %4.3f %.4f %.4f %.4f %.4f %d %d %0.4f %0.4f %0.4f\r\n",
//             mouse->range_data_m.back_left,
//             mouse->range_data_m.front_left,
//             mouse->range_data_m.gerald_left,
//             mouse->range_data_m.front,
//             mouse->range_data_m.gerald_right,
//             mouse->range_data_m.front_right,
//             mouse->range_data_m.back_right,
//             mouse->kinematic_controller.sense_left_wall,
//             mouse->kinematic_controller.sense_right_wall,
//             p.row,
//             p.col,
//             p.yaw);

  last_t_us = now_us;
}
