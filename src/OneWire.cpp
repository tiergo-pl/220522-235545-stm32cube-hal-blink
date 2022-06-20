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
  if (HAL_UART_Init(pHuart) != HAL_OK)
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
  uint8_t resetSignal = 0xf0;
  /*rxFunc = [this]()
  { resetConfirmed(); };*/
  uartSetBaud(9600);
  mPrevClk = *pClkSource;
  HAL_UART_Transmit(pHuart, &resetSignal, 1,1);
  HAL_UART_Receive(pHuart, mRXBuffer, 1,1);
  uint8_t resetConfirmation = mRXBuffer[0];
  if (resetConfirmation == 0xe0)
  {
    LOG_SWO("1-wire device present, reset confirmation= 0x%x\r\n", resetConfirmation);
  }
  else
  {
    LOG_SWO("1-wire device abnormal response, reset confirmation= 0x%x\r\n", resetConfirmation);
  }
  //mPrevClk = 0; // disable timeout calculation
  //rxFunc = nullptr;
  uartSetBaud(115200);
  mTXMessage[0] = 0x33;
  write(mTXMessage, 1);
  read(8);
  LOG_SWO("Received signature: 0x%2x%2x%2x%2x\r\n", mRXMessage[3], mRXMessage[2], mRXMessage[1], mRXMessage[0]);
}

void OneWire::write(uint8_t *txMessage, uint8_t messageLength)
{
  for (uint8_t i = 0; i < messageLength; ++i)
  {
    uint8_t shiftReg = txMessage[i];
    for (uint8_t j = 0; j < 8; ++j)
    {
      writeBit(shiftReg & 0x01);
      shiftReg = shiftReg >> 1;
    }
  }
}

uint8_t *OneWire::read(uint8_t messageLength)
{
  for (uint8_t i = 0; i < messageLength; ++i)
  {
    uint8_t shiftReg = 0;
    for (uint8_t j = 0; j < 7; ++j)
    {
      shiftReg |= readBit() << 7;
      shiftReg = shiftReg >> 1;
    }
    shiftReg |= readBit() << 7;
    mRXMessage[i] = shiftReg;
  }
  return mRXMessage;
}

void OneWire::writeBit(uint8_t value)
{
  uint8_t bit0 = 0x01;
  uint8_t bit1 = 0xff;
  if (value)
  {
    HAL_UART_Transmit(pHuart, &bit1, 1, 1);
  }
  else
  {
    HAL_UART_Transmit(pHuart, &bit0, 1, 1);
  }
}

uint8_t OneWire::readBit()
{
  uint8_t startPulse = 0xff;
  uint8_t recData = 0x0;
  //HAL_UART_Receive_IT(pHuart, &recData, 1);
  HAL_UART_Receive(pHuart, &recData, 1,1);//flush buffer
  HAL_UART_Transmit(pHuart, &startPulse,1,1);
  HAL_UART_Receive(pHuart, &recData, 1, 1);
  LOG_SWO("|%x", recData);
  return recData & 0x01;
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
