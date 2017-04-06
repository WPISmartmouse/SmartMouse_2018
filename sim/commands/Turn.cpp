#include "Turn.h"
#include "TurnInPlace.h"
#include "Forward.h"
#include "Stop.h"
#include "ForwardToCenter.h"
#include "ArcTurn.h"

Turn::Turn(Direction dir) : CommandGroup("SimTurnGroup"), mouse(SimMouse::inst()), dir(dir) {}

void Turn::initialize() {
  // if we want a logical 180 turn, we do full stop then turn.
  if (opposite_direction(mouse->getDir()) == dir) {
    addSequential(new Stop(100));
    addSequential(new TurnInPlace(dir));
    addSequential(new Stop(50));
    addSequential(new Forward());
  } else if (mouse->getDir() != dir) {
    addSequential(new ArcTurn(dir));
  }
//  if (dir != mouse->getDir()) {
//    addSequential(new ForwardToCenter()); // slowly stop
//    addSequential(new Stop(100)); // slowly stop
//    addSequential(new TurnInPlace(dir));
//    addSequential(new Forward());
//  }
}

