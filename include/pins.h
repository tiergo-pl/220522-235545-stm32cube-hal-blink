#pragma once
#include "main.h"

void pinSetup(GPIO_TypeDef *port, uint32_t pin, uint32_t mode, uint32_t pull, uint32_t speed);


class Key
{
private:
  enum BState
  {
    NotPressed,
    Debounce,
    ShortPressed,
    LongPressed
  };
  GPIO_TypeDef *port;
  uint16_t pin;
  BState buttonState;
  Callback shortPressFunc;
  Callback longPressingFunc;
  Callback longPressFunc;
  uint8_t pressedTime = 0;

public:
  Key(GPIO_TypeDef *_port, uint16_t _pin);

  void setFunction(Callback _shortPressFunc = nullptr, Callback _longPressingFunc = nullptr, Callback _longPressFunc = nullptr);
  void read();
};
