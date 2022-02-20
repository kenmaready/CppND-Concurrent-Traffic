#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 

    // lock while chaging (removing an element from) _queue
    std::unique_lock<std::mutex> uLock(_mutex);
    _condition.wait(uLock, [this] { return !_queue.empty(); }); // pass unique lock to condition variable

    // remove last message from _queue
    T msg = std::move(_queue.front());
    std::cout << "Removing msg " << (msg == TrafficLightPhase::red ? "red" : "green") << " to _queue..." << std::endl; // #DEBUG
    _queue.pop_front();

    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    
    // lock in order to modify (add to) vector
    std::lock_guard<std::mutex> guard(_mutex);

    // add vmessage to queue
    std::cout << "Adding msg " << (msg == TrafficLightPhase::red ? "red" : "green") << " to _queue..." << std::endl; // #DEBUG
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
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
}

TrafficLightPhase TrafficLight::getCurrentPhase() const
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles.
    

    std::chrono::steady_clock::time_point cycle_end = _getNewCycleEnd();

    while(true) {
        if (std::chrono::steady_clock::now() > cycle_end) {
            std::cout << "Cycle ended..." << std:: endl; // #DEBUG
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
        std::cout << "Changing to green" << std::endl; // #DEBUG
        _currentPhase = TrafficLightPhase::green;
    } else {
        std::cout << "Changing to red" << std::endl; // #DEBUG
        _currentPhase = TrafficLightPhase::red;
    }

    _phases.send(std::move(_currentPhase));
}