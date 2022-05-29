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

//#include "usb_device.h"
#include <stdio.h>
#include "pins.h"

void Error_Handler(void);
void SystemClock_Config(void);
uint32_t fastCounter = 0;
uint32_t prevFastCounter = 0;
/*
extern "C" uint32_t HAL_GetTick()
{
  return HAL_GetTick();
}*/

extern "C" int _write(int32_t file, uint8_t *ptr, int32_t len)
{
  int i = 0;
  for (i = 0; i < len; i++)
  {
    ITM_SendChar((*ptr++));
  }
  return len;
}
class CycleTimer
{
  private:
  uint32_t period;
  uint32_t duty;
  void (*funcStart)(void);
  void (*funcEnd)(void);
  uint32_t start;
  uint32_t end;
  public:
  CycleTimer(uint32_t _period, uint32_t _duty, void (*funcS)() = nullptr, void (*funcE)() = nullptr, uint32_t _start = 0, uint32_t _end = 0) : period(_period), duty(_duty), funcStart(funcS), funcEnd(funcE), start(_start), end(_end)
  {
    ;
  }
  void execute()
  {
  if (HAL_GetTick() >= start)
  {
    // printf("start= %lu, period= %lu\t", start, period);
    end = start + duty * period / 100;
    start += period;
    // printf("start= %lu, end= %lu\r\n", start, end);

    if (funcStart != nullptr)
    {
      funcStart();
    }
  }
  if (HAL_GetTick() >= end)
  {
    // printf("end= %lu\t", end);
    end += period;
    // printf("end= %lu\r\n", end);

    if (funcEnd != nullptr)
    {
      funcEnd();
    }
  }

  }
};
void ledPC13Toggle()
{
  HAL_GPIO_TogglePin(LED_PC13);
  printf("SysTick= %lu; LED toggled. fastCounter= %lu, fastCounter cycles= %lu\r\n", HAL_GetTick(), fastCounter, fastCounter - prevFastCounter);
  prevFastCounter = fastCounter;
}
void buzzerSet()
{
  HAL_GPIO_WritePin(BUZZER, GPIO_PIN_SET);
}
void buzzerReset()
{
  HAL_GPIO_WritePin(BUZZER, GPIO_PIN_RESET);
}

void ledDebugSet()
{
  HAL_GPIO_WritePin(LED_DEBUG, GPIO_PIN_SET);
  // printf("SysTick= %lu; LED_DEBUG HIGH\r\n", HAL_GetTick());
}
void ledDebugReset()
{
  HAL_GPIO_WritePin(LED_DEBUG, GPIO_PIN_RESET);
  // printf("SysTick= %lu; LED_DEBUG LOW\r\n", HAL_GetTick());
}

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  printf("Debug: line number %u\r\n", __LINE__);

  pinSetup(LED_PC13, LED_PC13_MODE);
  pinSetup(LED_DEBUG, LED_DEBUG_MODE);
  pinSetup(BUZZER, BUZZER_MODE);
  // MX_USB_DEVICE_Init();
  printf("Debug: line number %u\r\n", __LINE__);

  CycleTimer timerLEDPC13 = {PERIOD1, 50, &ledPC13Toggle};
  CycleTimer timerBuzzer = {PERIOD2, DUTY2, &buzzerSet, &buzzerReset};
  CycleTimer timerLEDDebug = {1000, 10, &ledDebugSet, &ledDebugReset, 400};

  printf("HalVersion= %lu; RevID= %lu; DevID= %lu; SystemCoreClock= %lu kHz\r\n", HAL_GetHalVersion(), HAL_GetREVID(), HAL_GetDEVID(), SystemCoreClock / 1000);
  while (1)
  {

    timerLEDPC13.execute() ;
    timerBuzzer.execute();
    timerLEDDebug.execute();
    fastCounter++;
  }
}

extern "C" void SysTick_Handler(void)
{
  HAL_IncTick();
}

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  while (1)
  {
  }
}

void BusFault_Handler(void)
{
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}
/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
