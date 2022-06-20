#pragma once
#include "main.h"

#define ONEWIRE_DEF_TIMEOUT 10
class OneWire
{
private:
  int32_t *pClkSource;
  UART_HandleTypeDef *pHuart;
  Callback rxFunc;
  int32_t mPrevClk = 0; // when 0 - don't count timeout, otherwise - var used to timeout calculation
  uint8_t mDataReady = 0;
  uint8_t mDevSignature[8];
  uint8_t mTXBuffer[64];

public:
  uint8_t mRXBuffer[64];
  uint8_t mTXMessage[10];
  uint8_t mRXMessage[10];

  OneWire(int32_t *clkSource, UART_HandleTypeDef *huart, USART_TypeDef *USART, uint32_t baudRate = 115200);
  void uartInit();
  void uartSetBaud(uint32_t baudrate);
  UART_HandleTypeDef *getHandle();

  void reset();
  uint8_t *readSignature();
  void measureTemp();
  float readTemp();

  void write(uint8_t *txMessage, uint8_t messageLength);
  void writeBit(uint8_t value);
  uint8_t *read(uint8_t messageLength);
  uint8_t readBit();

  uint8_t getDataReady() const;
  void setDataReady(const uint8_t &dataReady);

  const uint8_t *getRXBuffer() const;
};
