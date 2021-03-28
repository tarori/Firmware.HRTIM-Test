#pragma once
#include <cstdint>
#include "hrtim.h"
#include <stm32f3xx.h>

void init_common();
void HAL_HRTIM_Set_Compare(HRTIM_HandleTypeDef* hhrtim, uint32_t TimerIdx, uint32_t PWMChannel, uint32_t Pulse);
