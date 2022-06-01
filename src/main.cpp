

//#include "usb_device.h"
#include "main.h"
#include "pins.h"
#include "timers.h"

// Global vars ----------------------------------------------------------
void Error_Handler(void);
void SystemClock_Config(void);
uint32_t fastCounter = 0;
uint32_t prevFastCounter = 0;
int32_t tickWorkingCopy;
// ---------------------------------------------------------------------
/*extern "C" uint32_t HAL_GetTick()
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
void ledPC13Toggle()
{
  HAL_GPIO_TogglePin(LED_PC13);
  printf("SysTick= %lu; LED toggled. fastCounter= %lu, fastCounter cycles= %lu\r\n", HAL_GetTick(), fastCounter, fastCounter - prevFastCounter);
  prevFastCounter = fastCounter;
  //  printf("Debug: line number %u\r\n", __LINE__);
}
void buzzerSet()
{
  HAL_GPIO_WritePin(BUZZER, GPIO_PIN_SET);
  //  printf("Debug: line number %u\r\n", __LINE__);
}
void buzzerReset()
{
  HAL_GPIO_WritePin(BUZZER, GPIO_PIN_RESET);
  //  printf("Debug: line number %u\r\n", __LINE__);
}

void ledDebugSet()
{
  HAL_GPIO_WritePin(LED_DEBUG, GPIO_PIN_SET);
  // printf("Debug: line number %u\r\n", __LINE__);

  // printf("SysTick= %lu; LED_DEBUG HIGH\r\n", HAL_GetTick());
}
void ledDebugReset()
{
  HAL_GPIO_WritePin(LED_DEBUG, GPIO_PIN_RESET);
  //  printf("Debug: line number %u\r\n", __LINE__);

  // printf("SysTick= %lu; LED_DEBUG LOW\r\n", HAL_GetTick());
}
// ***************************************************************************************************************************************************
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

//  uwTick = (uint32_t)(-4200); // Testing only!
//  uwTick = 0x0fffffff - 4200; // Testing only!
  CycleTimer timerLEDPC13 = {&tickWorkingCopy, PERIOD1, 50, (int32_t)uwTick};
  timerLEDPC13.registerCallbacks(&ledPC13Toggle);
  CycleTimer timerBuzzer = {&tickWorkingCopy, PERIOD2, DUTY2, (int32_t)uwTick};
  timerBuzzer.registerCallbacks(&buzzerSet, &buzzerReset);
  CycleTimer timerLEDDebug = {&tickWorkingCopy, 1000, 10, 400 + (int32_t)uwTick};
  timerLEDDebug.registerCallbacks(&ledDebugSet, &ledDebugReset);

  printf("HalVersion= %lu; RevID= %lu; DevID= %lu; SystemCoreClock= %lu kHz\r\n", HAL_GetHalVersion(), HAL_GetREVID(), HAL_GetDEVID(), SystemCoreClock / 1000);
  while (1)
  {
    tickWorkingCopy = HAL_GetTick();
    timerLEDPC13.execute();
    timerBuzzer.execute();
    timerLEDDebug.execute();
    fastCounter++;
  }
}
// **************************************************************************************************************************************************************
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
