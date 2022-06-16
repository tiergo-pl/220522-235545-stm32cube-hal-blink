

//#include "usb_device.h"
#include "main.h"
#include "pins.h"
#include "timers.h"

// Global vars ----------------------------------------------------------
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

char uart2RxData;
uint8_t lineFeed = 0;
char uart3RxData;

uint32_t fastCounter = 0;
uint32_t prevFastCounter = 0;
int32_t tickWorkingCopy;
// ---------------------------------------------------------------------
void SystemClock_Config(void);
static void MX_USART3_UART_Init(uint32_t baudRate);
static void MX_USART2_UART_Init(void);

// Set SWO pin to sending printf texts
extern "C" int _write(int32_t file, uint8_t *ptr, int32_t len)
{
  int i = 0;
  for (i = 0; i < len; i++)
  {
    ITM_SendChar((*ptr++));
  }
  return len;
}
// ***************************************************************************************************************************************************

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_USART3_UART_Init(115200);
  MX_USART2_UART_Init();

  printf("Debug: line number %u\r\n", __LINE__);

  char uartTxData[128];
  uint16_t uartTxSize = 0;
  static uint16_t uartCounter = 0;

  HAL_UART_Receive_IT(&huart2, (uint8_t *)&uart2RxData, 1);
  //HAL_HalfDuplex_EnableReceiver(&huart3);
  HAL_UART_Receive_IT(&huart3, (uint8_t *)&uart3RxData, 1);

  pinSetup(LED_PC13, LED_PC13_MODE);
  pinSetup(LED_DEBUG, LED_DEBUG_MODE);
  pinSetup(BUZZER, BUZZER_MODE);
  pinSetup(BUTTON_LEFT, BUTTON_LEFT_MODE);
  pinSetup(BUTTON_RIGHT, BUTTON_RIGHT_MODE);
  // MX_USB_DEVICE_Init();
  printf("Debug: line number %u\r\n", __LINE__);

  //  uwTick = (uint32_t)(-4200); // Testing only!
  //  uwTick = 0x0fffffff - 4200; // Testing only!
  CycleTimer timerLEDPC13(&tickWorkingCopy, PERIOD1);
  timerLEDPC13.registerCallbacks([&]()
                                 {HAL_GPIO_TogglePin(LED_PC13);
                                 //printf("SysTick= %lu; LED toggled. fastCounter= %lu, fastCounter cycles= %lu\r\n", HAL_GetTick(), fastCounter, fastCounter - prevFastCounter);
                                 prevFastCounter = fastCounter; });
  CycleTimer timerBuzzer(&tickWorkingCopy, PERIOD2, DUTY2);
  timerBuzzer.registerCallbacks([]()
                                { HAL_GPIO_WritePin(BUZZER, GPIO_PIN_SET); },
                                []()
                                { HAL_GPIO_WritePin(BUZZER, GPIO_PIN_RESET); });
  CycleTimer timerLEDDebug(&tickWorkingCopy, 100, 10);
  timerLEDDebug.registerCallbacks([]()
                                  { HAL_GPIO_WritePin(LED_DEBUG, GPIO_PIN_SET); },
                                  []()
                                  { HAL_GPIO_WritePin(LED_DEBUG, GPIO_PIN_RESET); });
  CycleTimer timerBurst(&tickWorkingCopy, 10000);
  int pulses = 5;
  timerBurst.registerCallbacks([&]()
                               { timerLEDDebug.setPulses(pulses); }); // takes also local variable
  Key buttonLeft(BUTTON_LEFT);
  Key buttonRight(BUTTON_RIGHT);
  buttonLeft.setFunction([&]()
                         { timerBuzzer.setPulses(2); },
                         [&]()
                         { printf("L_B++\r\n"); },
                         [&]()
                         { printf("Left Button - Long pressed, TX:%d\r\n",uartCounter);
                         uartCounter++;
                         uartTxSize = sprintf(uartTxData, "UART TX test. Number of sent messages: %d.\r\n", uartCounter);
                         printf(uartTxData);
                         HAL_UART_Transmit_IT(&huart2, (uint8_t *)uartTxData, uartTxSize); });
  buttonRight.setFunction([&]()
                          { timerLEDDebug.setPulses(3); },
                          [&]()
                          { printf("R_B++\r\n"); },
                          [&]()
                          { printf("Right Button - Long pressed\r\n");
                            uartTxSize = sprintf(uartTxData, "UART3 TX test. SysTick: %lu.\r\n", HAL_GetTick());
                            //HAL_HalfDuplex_EnableTransmitter(&huart3);
                            HAL_UART_Transmit_IT(&huart3, (uint8_t *)uartTxData, uartTxSize);
                            //HAL_HalfDuplex_EnableReceiver(&huart3);
                            });

  CycleTimer keyRefresh(&tickWorkingCopy, KEY_REFRESH_RATE);
  keyRefresh.registerCallbacks([&]()
                               { buttonLeft.read();
                               buttonRight.read(); });
  timerLEDPC13.setPulses(TIMER_INFINITE, (int32_t)uwTick);
  timerBuzzer.setPulses(TIMER_MONO, (int32_t)uwTick);
  timerLEDDebug.setPulses(3);
  timerBurst.setPulses(TIMER_INFINITE, (int32_t)uwTick + 10000);
  keyRefresh.setPulses(TIMER_INFINITE, (int32_t)uwTick);

  printf("HalVersion= %lu; RevID= %lu; DevID= %lu; SystemCoreClock= %lu kHz\r\n", HAL_GetHalVersion(), HAL_GetREVID(), HAL_GetDEVID(), SystemCoreClock / 1000);
  while (1)
  {
    tickWorkingCopy = HAL_GetTick();
    timerLEDPC13.execute();
    timerBuzzer.execute();
    timerLEDDebug.execute();
    timerBurst.execute();
    keyRefresh.execute();

    if (lineFeed == 0x0a && __HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) == SET)
    {
      // uartTxData[0]= '\n';
      // HAL_UART_Transmit_IT(&huart2, (uint8_t *)uartTxData, 1);
      HAL_UART_Transmit_IT(&huart2, &lineFeed, 1);
      lineFeed = 0;
    }
    fastCounter++;
  }
}
// **************************************************************************************************************************************************************
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
/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */
}

/**
 * @brief USART3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART3_UART_Init(uint32_t baudRate)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = baudRate;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_HalfDuplex_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart == &huart2)
  { // echo port
    HAL_UART_Transmit_IT(huart, (uint8_t *)&uart2RxData, 1);
    printf("Uart2 received: %c\r\n", uart2RxData);
    if (uart2RxData == 0x0d)
    {
      lineFeed = 0x0a;
    }
    HAL_UART_Receive_IT(huart, (uint8_t *)&uart2RxData, 1); // Turn on receiving again
  }
  if (huart == &huart3)
  { // forward to SWO
    printf("Uart3 received: %c\r\n",uart3RxData);
    HAL_UART_Receive_IT(huart, (uint8_t *)&uart3RxData, 1); // Turn on receiving again
  }
}