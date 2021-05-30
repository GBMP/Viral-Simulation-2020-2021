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

#include "simulation.h"
#include <iostream>
#include <emscripten.h>
#include <math.h>
#include "LockdownMovementStrategy.cpp"
#include "RegularMovementStrategy.cpp"

// Initiated an instance of the Stategy classes for use in this file
corsim::MovementStrategy *strat;
corsim::LockdownMovementStrategy LSTRAT;
corsim::RegularMovementStrategy RSTRAT;

namespace corsim
{
// Bound the first argument (type of strategy) to variable in the constructor
// Also added the variables in the simulation.h file
// More change below in the Tick method
Simulation::Simulation(std::string strategy, int width, int height, std::unique_ptr<Canvas> canvas, std::unique_ptr<StatisticsHandler> sh) : 
    _strategy{strategy}, _sim_width{width}, _sim_height{height}, _canvas{std::move(canvas)}, _sh{std::move(sh)} {}

void Simulation::add_subject(Subject&& s)
{
    this->_subjects.emplace_back(std::move(s));
}

void Simulation::run()
{
    if(running)
    {
        return;
    }

    running = true;

    while(true)
    {
        this->tick();
        emscripten_sleep(tick_speed);
    }
}

int counter  = 0;
// A global flag to keep track if lockdown is initiated at certain tresholds
bool goLockdown = false;

void Simulation::tick()
{
    counter++;

    double dt = tick_speed / 10.0;

    std::vector<Subject*> collision_checker;

    for(Subject& s : _subjects)
    {
        collision_checker.emplace_back(&s);

        wall_collision(s);

        // A new method where I validate if a subject is infected or immune and for how long
        // If the subject is infected for a cetain time, as soons as that time has passed, it will turn immune for a new certain time.
        // And then goes back to being vulnerable, until infected agian.  See method below
        subject_better(s);
    }

    for(int i = collision_checker.size()-1; i < collision_checker.size(); i--)
    {
        Subject* current_checking = collision_checker.at(i);
        collision_checker.erase(collision_checker.end());

        for(Subject* s : collision_checker)
        {
            subject_collision(*current_checking, *s);
        }
    }

    int numberInfected = 0;
    // Created a integer variable to track iterations in the for loop
    int i = 1;

    for(Subject& s : _subjects)
    {
        // If the LockdownStrategy was chosen or lockdown was triggered because of too many infections,
        // the first condition will pass on the 75% of the population as the modulo of 4 on the current iterated subject is not equal to 0.
        // Else the RegularMovementStrategy is set on the subject in the SetTrajectory method
        if((_strategy == corsim::LOCKSTRAT || goLockdown) && (i % 4) != 0)
        {
            strat = &LSTRAT;
        }
        else
        {
            strat = &RSTRAT;
        }

        // The trajectory is set on the subject based on the chosen strategy. The starting coordinates and the direction of the subject is
        // set at random in the main.cpp class, this hasn't changed with the code I received.
        s.setTrajectory(strat, dt);

        // Count iteration
        i++;

        if(s.infected())
        {
            numberInfected++;
        }
    }

    // Something extra I wanted to add was a lockdown strategy when more then 60% of the population is infected.
    // When the infections drop below 15% of the population, the lockdown is lifted.
    if(numberInfected > (_subjects.size() * 0.6))
    {
        goLockdown = true;
        
    }
    else if (numberInfected < (_subjects.size() * 0.15)) 
    {
        goLockdown = false;
    }

    if(counter % 30 == 0)
    {
        _sh.get()->communicate_number_infected(counter/30,numberInfected);
    }

    draw_to_canvas();
}

void Simulation::draw_to_canvas()
{
    _canvas.get()->clear();
    _canvas.get()->draw_rectangle(0,0,1,_sim_height,BLACK);
    _canvas.get()->draw_rectangle(0,0,_sim_width,1,BLACK);
    _canvas.get()->draw_rectangle(0,_sim_height-1,_sim_width,1,BLACK);
    _canvas.get()->draw_rectangle(_sim_width-1,0,1,_sim_height,BLACK);

    for(Subject& s : _subjects)
    {
        CanvasColor c = BLUE;

        if(s.infected())
        {
            c = RED;
        }
        if(s.immune())
        {
            c = GREEN;
        }

        _canvas.get()->draw_ellipse(s.x(), s.y(), s.radius(), c);
    }
}

void Simulation::wall_collision(Subject& s)
{
    if (s.x() - s.radius() + s.dx() < 0 ||
        s.x() + s.radius() + s.dx() > _sim_width) {
        s.set_dx(s.dx() * -1);
    }
    if (s.y() - s.radius() + s.dy() < 0 ||
        s.y() + s.radius() + s.dy() > _sim_height) {
        s.set_dy(s.dy() * -1);
    }
    if (s.y() + s.radius() > _sim_height) {
        s.set_y(_sim_height - s.radius());
    }
    if (s.y() - s.radius() < 0) {
        s.set_y(s.radius());
    }
    if (s.x() + s.radius() > _sim_width) {
        s.set_x(_sim_width - s.radius());
    }
    if (s.x() - s.radius() < 0) {
        s.set_x(s.radius());
    }
}

double distance(Subject& s1, Subject& s2)
{
    return sqrt(pow(s1.x() - s2.x(),2) + pow(s1.y() - s2.y(),2));
}

void Simulation::subject_better(Subject& s)
{
    // If the subject is infected I set the current counter value in this simulation as a argument to the method in the subject class. I use that count 
    // to compare to the currentCount value on the subject itself, which is set on infection (see subject_collision method below), 
    // to determine how long a subject can be sick/infectious for.
    if(s.infected()){
        s.setImmune(counter);
    }

    // If the subject is immune, we use the setReady method to track how long the subject is immune, and if it passes a certain amount of count/time, we reset the subject
    // to be vulnerable again and so the loops start again.

    if(s.immune()){
        s.setReady(counter);
    }
}

void Simulation::subject_collision(Subject& s1, Subject& s2)
{
    double dist = distance(s1, s2);

    if(dist < s1.radius() + s2.radius())
    {
        // If one of the subjects in the collion is infected && none are immune,
        // the other will get infected and at the same time we set the current counter value from the simulation 
        // as a base counter value on the subject (currentCount), so we can use it to track time
        if((s1.infected() || s2.infected()) && (!s1.immune() || !s2.immune()))
        {

            s1.infect(true);
            s1.setCurrentCount(counter);
            s2.infect(true);
            s2.setCurrentCount(counter);
        }        

        double theta1 = s1.angle();
        double theta2 = s2.angle();
        double phi = atan2(s1.x() - s2.x(), s1.y() - s2.y());

        double dx1F = ((2.0*cos(theta2 - phi)) / 2) * cos(phi) + sin(theta1-phi) * cos(phi+M_PI/2.0);
        double dy1F = ((2.0*cos(theta2 - phi)) / 2) * sin(phi) + sin(theta1-phi) * sin(phi+M_PI/2.0);
        
        double dx2F = ((2.0*cos(theta1 - phi)) / 2) * cos(phi) + sin(theta2-phi) * cos(phi+M_PI/2.0);
        double dy2F = ((2.0*cos(theta1 - phi)) / 2) * sin(phi) + sin(theta2-phi) * sin(phi+M_PI/2.0);

        s1.set_dx(dx1F);                
        s1.set_dy(dy1F);                
        s2.set_dx(dx2F);                
        s2.set_dy(dy2F);

        static_collision(s1, s2, false);
    }
}

void Simulation::static_collision(Subject& s1, Subject& s2, bool emergency)
{
    double overlap = s1.radius() + s2.radius() - distance(s1, s2);
    Subject& smallerObject = s1.radius() < s2.radius() ? s1 : s2;
    Subject& biggerObject = s1.radius() > s2.radius() ? s1 : s2;

    if(emergency)
    {
        Subject& temp = smallerObject;
        smallerObject = biggerObject;
        biggerObject = temp;
    }

    double theta = atan2((biggerObject.y() - smallerObject.y()), (biggerObject.x() - smallerObject.x()));
    smallerObject.set_x(smallerObject.x() - overlap * cos(theta));
    smallerObject.set_y(smallerObject.y() - overlap * sin(theta));

    if (distance(s1, s2) < s1.radius() + s2.radius()) {
        if (!emergency)
        {
            static_collision(s1, s2, true);
        }
    }
}

}