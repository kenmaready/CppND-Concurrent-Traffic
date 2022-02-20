#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // lock while chaging (removing an element from) _queue
    std::unique_lock<std::mutex> uLock(_mutex);
    _condition.wait(uLock, [this] { return !_queue.empty(); }); // pass unique lock to condition variable

    // remove last message from _queue
    T msg = std::move(_queue.front());
    _queue.pop_front();

    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // lock in order to modify (add to) vector
    std::lock_guard<std::mutex> guard(_mutex);

    // add vmessage to queue
    _queue.push_back(std::move(msg));
    _condition.notify_one(); 
}


/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    while (true)
    {
        // receive wakes up when a new element is available in the queue
        TrafficLightPhase phase = _phases.receive();
        if (phase == TrafficLightPhase::green) return;
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase() const
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    std::chrono::steady_clock::time_point cycle_end = _getNewCycleEnd();

    while(true) {
        if (std::chrono::steady_clock::now() > cycle_end) {
            _toggleLight();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            cycle_end = _getNewCycleEnd();
        }
    } 
}

// helper function for cucleThroughPhases()
std::chrono::steady_clock::time_point TrafficLight::_getNewCycleEnd() const 
{
    std::chrono::steady_clock::time_point cycle_start = std::chrono::steady_clock::now();
    int cycle_duration = rand() % (_CYCLE_LENGTH_MAX - _CYCLE_LENGTH_MIN) + _CYCLE_LENGTH_MIN;
    return cycle_start + std::chrono::milliseconds(cycle_duration);

}

// helper function to toggle light between red and green:
void TrafficLight::_toggleLight() {
    if (_currentPhase == TrafficLightPhase::red) {
        _currentPhase = TrafficLightPhase::green;
    } else {
        _currentPhase = TrafficLightPhase::red;
    }

    _phases.send(std::move(_currentPhase));
}