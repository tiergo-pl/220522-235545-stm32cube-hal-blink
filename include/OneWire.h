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
  uint8_t mTXBuffer[256];
  uint8_t mDataReady = 0;

public:
    uint8_t mRXBuffer[256];
    
OneWire(int32_t *clkSource, UART_HandleTypeDef *huart, USART_TypeDef *USART, uint32_t baudRate = 115200);
  void uartInit();
  void uartSetBaud(uint32_t baudrate);
  UART_HandleTypeDef *getHandle();

  void reset();
  void resetConfirmed();
  void write(uint8_t *);
  uint8_t read(uint8_t *);
  void writeBit(uint8_t value);
  uint8_t readBit();
  void uartReceive();
  void checkTimeout(int timeout = ONEWIRE_DEF_TIMEOUT);

  uint8_t getDataReady() const;
  void setDataReady(const uint8_t &dataReady);

  const uint8_t *getRXBuffer() const;
};
