#pragma once
#include "main.h"
#include "pins.h"

#define ENABLE_DELAY 100 //min 80
class LCD
{
private:
  GPIO_TypeDef *pRsRwPort, *pEnablePort, *pData4bitPort;// 4 bit data bus pins have to be from the same port
  uint16_t mRsRwPins, mEnablePin, mData4bitPins;
  uint8_t rsRwPinOffset; // index of 1st pin from control RS and R/W pins
  uint8_t dataPinOffset; // index of 1st pin from 4 bit data bus
  void fastDelay(uint32_t delay)
  {
    for (volatile uint32_t i = 0; i < delay;++i)
      ;
  }

public:
  LCD(GPIO_TypeDef *rsRwPort, uint16_t rsRwPins, GPIO_TypeDef *enablePort, uint16_t enablePin, GPIO_TypeDef *data4bitPort, uint16_t data4bitPins);

  void init(uint32_t mode, uint32_t pull, uint32_t speed);

  void set4bitMode(uint8_t lineMode = 0x08);

  void clearDisplay();

  void writeCommand(uint8_t command);

  void writeData(uint8_t data);

  void writeByte(const uint8_t& data);

  void write4bit(const uint8_t &data);
};