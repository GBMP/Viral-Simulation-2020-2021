#pragma once

#include "MovementStrategy.cpp"

namespace corsim
{
    class RegularMovementStrategy : public MovementStrategy
    {
        public:
            double setTrajectory(double cor, double dir, double dt);
    };

} 