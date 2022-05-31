#pragma once
#include "main.h"

class CycleTimer
{
private:
  const uint32_t *tickSource;
  uint32_t period;
  uint32_t duty;
  uint32_t start;
  uint32_t end;
  void (*funcStart)(void) = nullptr;
  void (*funcEnd)(void) = nullptr;

public:
  CycleTimer(uint32_t *_tickSource, uint32_t _period, uint32_t _duty, uint32_t _start = 0);
  void registerCallbacks(void (*funcS)() = nullptr, void (*funcE)() = nullptr);
  void execute();
};

