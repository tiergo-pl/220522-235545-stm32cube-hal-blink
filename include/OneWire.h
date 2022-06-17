#pragma once
#include "main.h"

class OneWire
{
private:
  UART_HandleTypeDef *pHuart;

public:
  OneWire(UART_HandleTypeDef *huart, USART_TypeDef *USART, uint32_t baudRate = 115200);  
  void uartInit(USART_TypeDef *USART, uint32_t baudRate);
  void uartSetBaud(uint32_t baudrate);
  UART_HandleTypeDef *getHandle();

  void reset();
  void write(uint8_t *);
  uint8_t read(uint8_t *);
  void writeBit(uint8_t value);
  uint8_t readBit();
};
