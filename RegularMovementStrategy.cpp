// Return values from my overwritten method from the Interface class
#include "RegularMovementStrategy.h"

namespace corsim {

    double RegularMovementStrategy::setTrajectory(double cor, double dir, double dt) {
        return cor + dir * dt;
    };
}