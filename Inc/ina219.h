#pragma once

#include "stm32f4xx_hal.h"

void  INA219_Init(void);
float INA219_GetVoltage(void);
float INA219_GetCurrent(void);
float INA219_GetPower(void);
