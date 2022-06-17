#pragma once
#include "main.h"

class OneWire
{
private:

public:
  OneWire();
  void reset();
  void write(uint8_t *);
  uint8_t read(uint8_t *);
  void writeBit(uint8_t value);
  uint8_t readBit();
};
