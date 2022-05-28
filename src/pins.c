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
