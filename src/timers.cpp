#include "timers.h"

CycleTimer::CycleTimer(int32_t *_tickSource, uint32_t _period, uint32_t _duty)
    : tickSource(_tickSource), period(_period), duty(_duty)
{
  end = start + duty * period / 100;
}

void CycleTimer::registerCallbacks(Callback funcS, Callback funcE)
{
  funcStart = funcS;
  funcEnd = funcE;
}

void CycleTimer::execute()
{
  if (pulses)
  {
    if (*tickSource - start >= 0)
    {
      //printf("hex SysTick= %lx; start= %lu, period= %lu\t", *tickSource, start, period);
      end = start + duty * period / 100;
      start += period;
      //printf("start= %lu, end= %lu\r\n", start, end);

      if (funcStart != nullptr)
      {
        funcStart();
      }
    }
    if (*tickSource - end >= 0)
    {
      //printf("hex SysTick= %lx; end= %lu\t", *tickSource, end);
      end += period;
      //printf("end= %lu\r\n", end);

      if (funcEnd != nullptr)
      {
        funcEnd();
      }
      if (pulses != -1)
      {
        pulses--;
      }
    }
  }
}

void CycleTimer::setPulses(int _pulses, int32_t _start)
{
  pulses = _pulses;
  if (_start)
  {
    start = _start;
  }
  else
  {
    start = *tickSource;
  }
}
