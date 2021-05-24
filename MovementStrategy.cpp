#pragma once
#include <string>

namespace corsim {
static const std::string REGSTRAT = "RegularMovementStrategy";
static const std::string LOCKSTRAT = "LockdownMovementStrategy";
    class MovementStrategy 
    {
        public:
            virtual double setTrajectory(double cor, double dir, double dt) = 0;
    };
}