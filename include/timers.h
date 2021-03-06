#pragma once
#include "main.h"

#define TIMER_DISABLE 0
#define TIMER_MONO 1
#define TIMER_INFINITE -1

class CycleTimer
{
private:
  const int32_t *tickSource;
  uint32_t period;
  uint32_t duty;
  int32_t start;
  int32_t end;
  Callback funcStart = nullptr;
  Callback funcEnd = nullptr;
  int pulses; // 0- timer disabled, 1 - one pulse, -1 - infinite, n - n pulses

public:
  CycleTimer(int32_t *_tickSource, uint32_t _period, uint32_t _duty = 50);
  void registerCallbacks(Callback funcS = nullptr, Callback funcE = nullptr);
  void execute();
  void setPulses(int _pulses, int32_t _start = 0);


};

