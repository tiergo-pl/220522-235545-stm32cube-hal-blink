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

ErrorStatus OneWire::reset()
{
  uint8_t resetSignal = 0xf0;
  /*rxFunc = [this]()
  { resetConfirmed(); };*/
  uartSetBaud(9600);
  mPrevClk = *pClkSource;
  HAL_UART_Transmit(pHuart, &resetSignal, 1, 1);
  HAL_UART_Receive(pHuart, mRXBuffer, 1, 1);
  uartSetBaud(115200);
  uint8_t resetConfirmation = mRXBuffer[0];
  if (resetConfirmation != 0xf0)
  {
    LOG_SWO("1-wire device present, reset confirmation= 0x%x\r\n", resetConfirmation);
    return SUCCESS;
  }
  else
  {
    LOG_SWO("1-wire device abnormal response, reset confirmation= 0x%x\r\n", resetConfirmation);
    return ERROR;
  }
  // mPrevClk = 0; // disable timeout calculation
  // rxFunc = nullptr;
}

uint8_t *OneWire::readSignature()
{
  if (reset() == ERROR)
  {
    return nullptr;
  }
  else
  {
    mTXMessage[0] = ONEWIRE_READ_ROM;
    write(mTXMessage, 1);
    read(8);
    for (uint8_t i = 0; i < 8; ++i)
    {
      mDevSignature[i] = mRXMessage[i];
    }
    if (crc(mRXMessage, 8) != 0) // crc=0 when no error
    {
      return nullptr;
    }

    uint32_t *_32bitSignature = (uint32_t *)mDevSignature;
    LOG_SWO("Received signature: 0x%08lx.%08lx\r\n", _32bitSignature[1], _32bitSignature[0]);
    return mDevSignature;
  }
}

ErrorStatus OneWire::measureTemp()
{
  if (reset() == ERROR)
  {
    return ERROR;
  }
  else
  {
    mTXMessage[0] = ONEWIRE_SKIP_ROM;
    mTXMessage[1] = ONEWIRE_CONVERT_T;
    write(mTXMessage, 2);
    return SUCCESS;
  }
}

float OneWire::readTemp(uint8_t *devSignature)
{
  if (reset() == SUCCESS)
  {
    if (devSignature == nullptr) // when reading only one sensor
    {
      mTXMessage[0] = ONEWIRE_SKIP_ROM;
      mTXMessage[1] = ONEWIRE_READ_SCRATCHPAD;
      write(mTXMessage, 2);
    }
    else
    {
      mTXMessage[0] = ONEWIRE_MATCH_ROM; // when multi sensor bus reading
      for (uint8_t i = 0; i < 8; ++i)
      {
        mTXMessage[i + 1] = devSignature[i];
      }
      mTXMessage[9] = ONEWIRE_READ_SCRATCHPAD;
      write(mTXMessage, 10);
    }
    read(9);
    LOG_SWO("Received scratchpad: 0x%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x\r\n",
            mRXMessage[8], mRXMessage[7], mRXMessage[6], mRXMessage[5], mRXMessage[4], mRXMessage[3], mRXMessage[2], mRXMessage[1], mRXMessage[0]);
    if (crc(mRXMessage, 9) != 0) // crc=0 when no error 
    {
      return -400;
    }

    int16_t *intTemp = (int16_t *)mRXMessage;
    float temperature = intTemp[0] / 16.0f;
    LOG_SWO("intTemp: %d, Temperature: %.1f*C\r\n", intTemp[0], temperature);
    return temperature;
  }
  else
  {
    return -333;
  }
}

uint8_t OneWire::crc(uint8_t *data, uint8_t nbrOfBytes)
{
  uint8_t crc = 0;
  uint8_t byteCtr;
  // calculates 8-Bit checksum
  for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
  {

    crc ^= (data[byteCtr]);
    for (uint8_t bit = 0; bit < 8; ++bit)
    {
      if (crc & 0x01)
        crc = (crc >> 1) ^ 0x8c; // polynomial Maxim 0x31 - reversed order
      else
        crc = (crc >> 1);
    }
  }
  LOG_SWO("CRC= 0x%02x\r\n", crc);
  return crc;
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

uint8_t *OneWire::read(uint8_t messageLength)
{
  uint8_t trash;
  HAL_UART_Receive(pHuart, &trash, 1, 1); // flush buffer
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

uint8_t OneWire::readBit()
{
  uint8_t startPulse = 0xff;
  uint8_t recData = 0x0;
  // HAL_UART_Receive_IT(pHuart, &recData, 1);
  HAL_UART_Transmit(pHuart, &startPulse, 1, 1);
  HAL_UART_Receive(pHuart, &recData, 1, 1);
  // LOG_SWO("|%x", recData);
  return recData & 0x01;
}

uint8_t OneWire::getDataReady() const { return mDataReady; }

void OneWire::setDataReady(const uint8_t &dataReady) { mDataReady = dataReady; }

const uint8_t *OneWire::getRXBuffer() const { return mRXBuffer; }
