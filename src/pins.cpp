#include "pins.h"
void pinSetup(GPIO_TypeDef *port, uint32_t pin, uint32_t mode, uint32_t pull, uint32_t speed)
{
  if (port == GPIOA)
  {
    __HAL_RCC_GPIOA_CLK_ENABLE();
  }
  if (port == GPIOB)
  {
    __HAL_RCC_GPIOB_CLK_ENABLE();
  }
  if (port == GPIOC)
  {
    __HAL_RCC_GPIOC_CLK_ENABLE();
  }
  GPIO_InitTypeDef GPIO_InitStruct;

  GPIO_InitStruct.Pin = pin;
  GPIO_InitStruct.Mode = mode;
  GPIO_InitStruct.Pull = pull;
  GPIO_InitStruct.Speed = speed;
  HAL_GPIO_Init(port, &GPIO_InitStruct);
}

Key::Key(GPIO_TypeDef *_port, uint16_t _pin) : port(_port), pin(_pin)
{
}
void Key::setFunction(Callback _shortPressFunc, Callback _longPressingFunc, Callback _longPressFunc)
{
  shortPressFunc = _shortPressFunc;
  longPressingFunc = _longPressingFunc;
  longPressFunc = _longPressFunc;
}
void Key::read()
{
  if (HAL_GPIO_ReadPin(port, pin) == KEY_PRESSED)
  {
    switch (buttonState)
    {
    case NotPressed:
        buttonState = Debounce;
      break;
    case Debounce:
      buttonState = ShortPressed;
      break;
    case ShortPressed:
    case LongPressed:
      if (pressedTime > LONG_PRESS_DURATION/KEY_REFRESH_RATE) // change to relative value
      {
        buttonState = LongPressed;
        if (longPressingFunc != nullptr)
          longPressingFunc();
      }
      break;

    default:
      break;
    }
    pressedTime++;
  }
  else
  {
    switch (buttonState)
    {
    case ShortPressed:
      if (shortPressFunc != nullptr)
        shortPressFunc();
      break;
    case LongPressed:
      if (longPressFunc != nullptr)
        longPressFunc();
      break;
    default:
      break;
    }

    buttonState = NotPressed;
    pressedTime = 0;
  }
}
