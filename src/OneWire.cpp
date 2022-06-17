#include "OneWire.h"

/**
 * @brief Create OneWire object on uart TX pin
 * @param huart pointer to huart structure, e.g. huart3
 * @param baudRate baud rate
 * @retval None
 */
OneWire::OneWire(UART_HandleTypeDef *huart, USART_TypeDef *USART, uint32_t baudRate) : pHuart(huart)
{
  uartInit(USART, baudRate);
}

/**
 * @brief OneWire USART Initialization Function - Half duplex mode on uart TX pin
 * @param huart pointer to huart structure, e.g. huart3
 * @param baudRate baud rate
 * @retval None
 */
void OneWire::uartInit(USART_TypeDef *USART, uint32_t baudRate)
{
  // From stm32CubeIde
  pHuart->Instance = USART;
  pHuart->Init.BaudRate = baudRate;
  pHuart->Init.WordLength = UART_WORDLENGTH_8B;
  pHuart->Init.StopBits = UART_STOPBITS_1;
  pHuart->Init.Parity = UART_PARITY_NONE;
  pHuart->Init.Mode = UART_MODE_TX_RX;
  pHuart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  pHuart->Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_HalfDuplex_Init(pHuart) != HAL_OK)
  {
    Error_Handler();
  }
}
/**
 * @brief OneWire USART baud rate setting
 * @param baudRate baud rate
 * @retval None
 */

void OneWire::uartSetBaud(uint32_t baudrate)
{
  uartInit(pHuart->Instance, baudrate);
}

UART_HandleTypeDef *OneWire::getHandle()
{
  return pHuart;
}

void OneWire::reset()
{
  uartSetBaud(9600);

  uartSetBaud(115200);
}
