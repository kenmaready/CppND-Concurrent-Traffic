#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"


// forward declarations to avoid include cycle
class Vehicle;

// enum for use in classes below
enum class TrafficLightPhase { red, green }; 

template <class T>
class MessageQueue
{
public:
    void send(T &&msg);
    T receive();

private:
    std::deque<T> _queue;
    std::condition_variable _condition;
    std::mutex _mutex;
};


class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase() const;

    // typical behaviour methods
    void waitForGreen();
    void simulate();

private:
    // typical behaviour methods
    void cycleThroughPhases(); //

    // helper functions for cycleThroughPhases
    std::chrono::steady_clock::time_point _getNewCycleEnd() const;
    void _toggleLight();

    // private member variables
    std::condition_variable _condition;
    std::mutex _mutex;
    TrafficLightPhase _currentPhase;
    MessageQueue<TrafficLightPhase> _phases;
    int _CYCLE_LENGTH_MIN = 4000; // low end (in ms) of light cycle
    int _CYCLE_LENGTH_MAX = 6000; // high end (in ms) of light cycle
};

#endif