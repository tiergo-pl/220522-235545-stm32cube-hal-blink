#include "OneWire.h"

void OneWire::reset()
{
  MX_USART3_UART_Init(9600);

  MX_USART3_UART_Init(115200);
}
