#include "TrafficLight.h"
#include <iostream>
#include <random>

template <typename T> T MessageQueue<T>::receive() {
  std::unique_lock<std::mutex> lock(_mutex);
  _condition.wait(lock, [this]() { return !_queue.empty(); });
  TrafficLightPhase curr_phase = _queue.front();
  _queue.pop_front();
  return curr_phase;
}

template <typename T> void MessageQueue<T>::send(T &&msg) {
  std::lock_guard<std::mutex> guard(_mutex);
  _queue.push_back(msg);
  _condition.notify_one();
}

TrafficLight::TrafficLight() { _currentPhase = TrafficLightPhase::red; }

void TrafficLight::waitForGreen() {
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    TrafficLightPhase curr_phase = _queue.receive();
    if (curr_phase == TrafficLightPhase::green) {
      return;
    }
  }
}

TrafficLightPhase TrafficLight::getCurrentPhase() { return _currentPhase; }

void TrafficLight::simulate() {
  threads.push_back(std::thread([this]() { cycleThroughPhases(); }));
}

void TrafficLight::cycleThroughPhases() {
  double cycleDuration = (rand() % 3 + 4) * 1000;
  std::chrono::time_point<std::chrono::system_clock> lastUpdate;

  lastUpdate = std::chrono::system_clock::now();
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    long timeSinceLastUpdate =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - lastUpdate)
            .count();
    if (timeSinceLastUpdate >= cycleDuration) {
      _currentPhase = _currentPhase == TrafficLightPhase::green
                          ? TrafficLightPhase::red
                          : TrafficLightPhase::green;

      _queue.send(std::move(_currentPhase));

      lastUpdate = std::chrono::system_clock::now();
    }
  }
}
