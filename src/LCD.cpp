#include "LCD.h"

LCD::LCD(GPIO_TypeDef *rsRwPort, uint16_t rsRwPins, GPIO_TypeDef *enablePort, uint16_t enablePin, GPIO_TypeDef *data4bitPort, uint16_t data4bitPins)
    : pRsRwPort(rsRwPort), pEnablePort(enablePort), pData4bitPort(data4bitPort), mRsRwPins(rsRwPins), mEnablePin(enablePin), mData4bitPins(data4bitPins)
{
  ;
}

void LCD::init(uint32_t mode, uint32_t pull, uint32_t speed)
{
  pinSetup(pRsRwPort, mRsRwPins, mode, pull, speed);
  pinSetup(pEnablePort, mEnablePin, mode, pull, speed);
  pinSetup(pData4bitPort, mData4bitPins, mode, pull, speed);
  dataPinOffset = 0;
  while (((mData4bitPins >> dataPinOffset) & 0x01) == 0) // index of LSB in 4-bit bus
    ++dataPinOffset;
  rsRwPinOffset = 0;
  while (((mRsRwPins >> rsRwPinOffset) & 0x01) == 0) // index of RS pin
    ++rsRwPinOffset;
  fastDelay(1);
}

void LCD::set4bitMode(uint8_t lineMode)
{
  writeCommand(0x20);
  writeCommand(0x20 | lineMode);
}

void LCD::clearDisplay()
{
  writeCommand(0x01);
  fastDelay(100000);
}

void LCD::writeCommand(uint8_t command)
{
  HAL_GPIO_WritePin(pRsRwPort, mRsRwPins, GPIO_PIN_RESET);
  writeByte(command);
}

void LCD::writeData(uint8_t data)
{
  HAL_GPIO_WritePin(pRsRwPort, mRsRwPins, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(pRsRwPort, 1 << rsRwPinOffset, GPIO_PIN_SET);
  writeByte(data);
}

void LCD::writeByte(const uint8_t &data)
{
  uint8_t upper, lower;
  upper = (data & 0xf0) >> 4;
  lower = data & 0x0f;
  write4bit(upper);
  write4bit(lower);
}

void LCD::write4bit(const uint8_t &data)
{
  fastDelay(ENABLE_DELAY);
  HAL_GPIO_WritePin(pEnablePort, mEnablePin, GPIO_PIN_SET);
  fastDelay(ENABLE_DELAY);
  HAL_GPIO_WritePin(pData4bitPort, ((~data) & 0x0f) << dataPinOffset, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(pData4bitPort, (data) << dataPinOffset, GPIO_PIN_SET);
  fastDelay(ENABLE_DELAY);
  HAL_GPIO_WritePin(pEnablePort, mEnablePin, GPIO_PIN_RESET);
}
