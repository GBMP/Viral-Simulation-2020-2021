//interface class for our method to set locaiont, direction and speed.

#pragma once
#include <string>

namespace corsim 
{
    //Two constants to help with picking the correct strategy for the simulation
    static const std::string REGSTRAT = "RegularMovementStrategy";
    static const std::string LOCKSTRAT = "LockdownMovementStrategy";
    class MovementStrategy 
    {
        public:
            virtual double setTrajectory(double cor, double dir, double dt) = 0;
    };
}