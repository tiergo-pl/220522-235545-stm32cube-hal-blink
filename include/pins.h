#pragma once
#include "main.h"

void pinSetup(GPIO_TypeDef *port, uint32_t pin, uint32_t mode, uint32_t pull, uint32_t speed);
