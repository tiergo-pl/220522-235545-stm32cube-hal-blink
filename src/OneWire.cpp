#include "OneWire.h"

/**
 * @brief Create OneWire object on uart TX pin
 * @param huart pointer to huart structure, e.g. huart3
 * @param baudRate baud rate
 * @retval None
 */
OneWire::OneWire(int32_t *clkSource, UART_HandleTypeDef *huart, USART_TypeDef *USART, uint32_t baudRate) : pClkSource(clkSource), pHuart(huart)
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
}

/**
 * @brief OneWire USART Initialization Function - Half duplex mode on uart TX pin
 * @param
 * @retval None
 */
void OneWire::uartInit()
{
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

void OneWire::uartSetBaud(uint32_t baudRate)
{
  pHuart->Init.BaudRate = baudRate;
  uartInit();
}

UART_HandleTypeDef *OneWire::getHandle()
{
  return pHuart;
}

void OneWire::reset()
{
  uint8_t resetSignal = 0x00;
  rxFunc = [this]()
  { resetConfirmed(); };
  uartSetBaud(19200);
  mPrevClk = *pClkSource;
  HAL_UART_Transmit_IT(pHuart, &resetSignal, 1);
  HAL_UART_Receive_IT(pHuart, mRXBuffer, 1);
}

void OneWire::resetConfirmed()
{
  uint8_t resetConfirmation = mRXBuffer[0];
  if (resetConfirmation <= 0xff && resetConfirmation >= 0xfc)
  {
    LOG_SWO("1-wire device present, reset confirmation= 0x%x\r\n", resetConfirmation);
  }
  else
  {
    LOG_SWO("1-wire device abnormal response, reset confirmation= 0x%x\r\n", resetConfirmation);
  }
  mPrevClk = 0; // disable timeout calculation
  uartSetBaud(115200);
}

void OneWire::uartReceive()
{
  if (mDataReady)
  {
    if (rxFunc != nullptr)
    {
      rxFunc();
    }
    mDataReady = 0;
  }
  else
  {
    checkTimeout();
  }
}

void OneWire::checkTimeout(int timeout)
{
  if (*pClkSource - mPrevClk > timeout && mPrevClk != 0)
  {
    LOG_SWO("No response - device unconnected\r\n");
    mPrevClk = 0; // disable timeout calculation
  }
}

uint8_t OneWire::getDataReady() const { return mDataReady; }

void OneWire::setDataReady(const uint8_t &dataReady) { mDataReady = dataReady; }

const uint8_t *OneWire::getRXBuffer() const { return mRXBuffer; }
