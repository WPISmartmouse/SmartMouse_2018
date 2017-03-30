#include "NavTestCommand.h"
#include "Forward.h"
#include "Stop.h"
#include "Turn.h"

NavTestCommand::NavTestCommand() : CommandGroup("NavTestGroup") {
  addSequential(new Forward());
  for (int i = 0; i < 4; i++) {
    addSequential(new Forward());
    addSequential(new Turn(Direction::W));
    addSequential(new Forward());
    addSequential(new Turn(Direction::E));
  }
  addSequential(new Stop(0));
}
