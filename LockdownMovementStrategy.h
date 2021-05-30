// Header file for my LockdownMovementStrategy class
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