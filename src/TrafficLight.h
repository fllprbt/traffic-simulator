#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include "TrafficObject.h"
#include <condition_variable>
#include <deque>
#include <mutex>

enum TrafficLightPhase { red, green };

// forward declarations to avoid include cycle
class Vehicle;

template <class T> class MessageQueue {
public:
  void send(T &&msg);
  T receive();

private:
  std::deque<TrafficLightPhase> _queue;
  std::condition_variable _condition;
  std::mutex _mutex;
};

class TrafficLight : public TrafficObject {
public:
  TrafficLight();
  TrafficLightPhase getCurrentPhase();
  void waitForGreen();
  void simulate();

private:
  void cycleThroughPhases();
  TrafficLightPhase _currentPhase;
  MessageQueue<TrafficLightPhase> _queue;
  std::condition_variable _condition;
  std::mutex _mutex;
};

#endif