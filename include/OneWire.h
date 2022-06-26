#pragma once
#include "main.h"

#define ONEWIRE_DEF_TIMEOUT 10
#define ONEWIRE_SKIP_ROM 0xcc
#define ONEWIRE_READ_ROM 0x33
#define ONEWIRE_SEARCH_ROM 0xf0
#define ONEWIRE_MATCH_ROM 0x55
#define ONEWIRE_CONVERT_T 0x44
#define ONEWIRE_READ_SCRATCHPAD 0xbe

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

  ErrorStatus reset();

  uint8_t *readSignature();

  ErrorStatus measureTemp();

  // @param devSignature pointer to 8-byte device signature; when using only one device on bus you can leave empty
  float readTemp(uint8_t *devSignature = nullptr);

  void write(uint8_t *txMessage, uint8_t messageLength);
  void writeBit(uint8_t value);
  uint8_t *read(uint8_t messageLength);
  uint8_t readBit();

  uint8_t getDataReady() const;
  void setDataReady(const uint8_t &dataReady);

  const uint8_t *getRXBuffer() const;


};
