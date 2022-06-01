#include "timers.h"

CycleTimer::CycleTimer(int32_t *_tickSource, uint32_t _period, uint32_t _duty, int32_t _start)
    : tickSource(_tickSource), period(_period), duty(_duty), start(_start)
{
  end = start + duty * period / 100;
}

void CycleTimer::registerCallbacks(void (*funcS)(), void (*funcE)())
{
  funcStart = funcS;
  funcEnd = funcE;
}

void CycleTimer::execute()
{
  if (*tickSource - start >= 0)
  {
    printf("hex SysTick= %x; start= %lu, period= %lu\t", *tickSource, start, period);
    end = start + duty * period / 100;
    start += period;
    printf("start= %lu, end= %lu\r\n", start, end);

    if (funcStart != nullptr)
    {
      funcStart();
    }
  }
  if (*tickSource - end >= 0)
  {
    printf("hex SysTick= %x; end= %lu\t", *tickSource, end);
    end += period;
    printf("end= %lu\r\n", end);

    if (funcEnd != nullptr)
    {
      funcEnd();
    }
  }
}
