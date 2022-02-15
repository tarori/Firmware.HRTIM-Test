#include "main_cycle.hpp"
#include "utils.hpp"
#include "hrtim.h"
#include <stm32f3xx.h>

void cycle_callback()
{
    static uint32_t counter = 0;
    ++counter;
    if (counter % 1000 < 500) {
        HAL_HRTIM_Set_Compare(&hhrtim1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_OUTPUT_TA1, 1000);

        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    } else {
        HAL_HRTIM_Set_Compare(&hhrtim1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_OUTPUT_TA1, 2000);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    }
}
