#pragma once

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
#include <functional>

#ifdef BLUEPILL_FAKE
#define LED_PC13 GPIOC, GPIO_PIN_13
#define LED_PC13_MODE GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW
#define LED_DEBUG GPIOA, GPIO_PIN_15
#define LED_DEBUG_MODE GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM
#define BUZZER GPIOB, GPIO_PIN_4
#define BUZZER_MODE GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_MEDIUM
#define BUTTON_LEFT GPIOB, GPIO_PIN_12
#define BUTTON_LEFT_MODE GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_MEDIUM
#define BUTTON_RIGHT GPIOB, GPIO_PIN_13
#define BUTTON_RIGHT_MODE GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_MEDIUM
#define KEY_PRESSED 0 //0 if button shorts to GND with pin pulled up to V+ 
#else
#define LED_PIN GPIO_PIN_5
#define LED_GPIO_PORT GPIOA
#define LED_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#endif
#define PERIOD1 500
#define PERIOD2 100
#define DUTY2 10
#define KEY_REFRESH_RATE 40 //in SysTicks (mSecs)

// Global vars ----------------------------------------------------------
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

extern char uart2RxData;
extern uint8_t lineFeed;
extern char uart3RxData;

extern uint32_t fastCounter;
extern uint32_t prevFastCounter;
extern int32_t tickWorkingCopy;
// ---------------------------------------------------------------------
void SystemClock_Config(void);
void MX_USART3_UART_Init(uint32_t baudRate);
void MX_USART2_UART_Init(void);

void Error_Handler(void);

using Callback = std::function<void()>;