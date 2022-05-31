#if F0
#include "stm32f0xx_hal.h"
#elif F1
#include "stm32f1xx_hal.h"
#elif F2
#include "stm32f2xx_hal.h"
#elif F3
#include "stm32f3xx_hal.h"
#elif F4
#include "stm32f4xx_hal.h"
#elif F7
#include "stm32f7xx_hal.h"
#elif L0
#include "stm32l0xx_hal.h"
#elif L1
#include "stm32l1xx_hal.h"
#elif L4
#include "stm32l4xx_hal.h"
#else
#error "Unsupported STM32 Family"
#endif

#include <stdio.h>

#ifdef BLUEPILL_FAKE
#define LED_PC13 GPIOC, GPIO_PIN_13
#define LED_PC13_MODE GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW
#define LED_DEBUG GPIOA, GPIO_PIN_12
#define LED_DEBUG_MODE GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM
#define BUZZER GPIOB, GPIO_PIN_5
#define BUZZER_MODE GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_MEDIUM

#else
#define LED_PIN GPIO_PIN_5
#define LED_GPIO_PORT GPIOA
#define LED_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#endif
#define PERIOD1 500
#define PERIOD2 1000
#define DUTY2 1
