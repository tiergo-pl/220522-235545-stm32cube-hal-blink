#pragma once
#include "main.h"

class CycleTimer
{
private:
  const int32_t *tickSource;
  uint32_t period;
  uint32_t duty;
  int32_t start;
  int32_t end;
  void (*funcStart)(void) = nullptr;
  void (*funcEnd)(void) = nullptr;

public:
  CycleTimer(int32_t *_tickSource, uint32_t _period, uint32_t _duty, int32_t _start = 0);
  void registerCallbacks(void (*funcS)() = nullptr, void (*funcE)() = nullptr);
  void execute();
};

