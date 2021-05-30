//Header file for our lockdownmovement strategy
#pragma once

#include "MovementStrategy.cpp"

namespace corsim
{
    class LockdownMovementStrategy : public MovementStrategy
    {
        public:
            double setTrajectory(double cor, double dir, double dt);
    };

} 