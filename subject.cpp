// Corona Simulation - basic simulation of a human transmissable virus
// Copyright (C) 2020  wbrinksma

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "subject.h"
#include <math.h>

namespace corsim
{

Subject::Subject(int x, int y, int radius, bool infected)
{
    this->_x = x;
    this->_y = y;
    this->_radius = radius;
    this->_infected = infected;
}

double Subject::x()
{
    return this->_x;
}

double Subject::y()
{
    return this->_y;
}

void Subject::set_x(double x)
{
    this->_x = x;
}

void Subject::set_y(double y)
{
    this->_y = y;
}

double Subject::dx()
{
    return this->_dx;
}

double Subject::dy()
{
    return this->_dy;
}

void Subject::set_dx(double dx)
{
    this->_dx = dx;
}

void Subject::set_dy(double dy)
{
    this->_dy = dy;
}

int Subject::radius()
{
    return this->_radius;
}

bool Subject::infected()
{
    return this->_infected;
}

bool Subject::immune()
{
    return this->_immune;
}

void Subject::infect(bool infect)
{
    this->_infected = infect;
}

double Subject::angle()
{
    return atan2(_dy,_dx);
}

double Subject::speed()
{
    return sqrt(_dx * _dx + _dy * _dy);
}

// Method to set the currentCount from the running simulation on the subject
void Subject::setCurrentCount(int count)
{
    this->_currentCount = count;
}

// The setImmune method compares the current counter value from the simulator class to the currentCount value that was set on infection on the subject.
// If that comparison passes a certain amount,
// the subject is set to immune (green) and false on infected,
// the currentCount on the subject is set to the argument passed so we can start tracking the immune period
void Subject::setImmune(int count)
{
    if(count - _currentCount == 150)
    {
        this->_immune = true;
        this->_infected = false;
        _currentCount = count;
    }
}

// The setReady method resets the subject back to normal (blue) so that it can be infected again.
// The counter from the running simulation class is used to compare to the currentCount that was set when the subject was made immune.
// If the condition passes, the immune value is set back to false and the currentCount on the subject is reset.
void Subject::setReady(int count)
{
    if(count - _currentCount == 750)
    {
        this->_immune = false;
        _currentCount = 0;
    }
}

// The setTrajectory method is the overwritten method from the Interface class, it sets the coordinates, direction and the speed in the graphics whenever it is iterated over,
// depending on the strategy class that is passed in the method, the correct value is returned.
void Subject::setTrajectory(MovementStrategy *strat, double dt)
{
    this->set_x(strat->setTrajectory(this->x(), this->dx(), dt));
    this->set_y(strat->setTrajectory(this->y(), this->dy(), dt));
}

}