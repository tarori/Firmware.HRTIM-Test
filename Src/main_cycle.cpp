#include "main_cycle.hpp"
#include "utils.hpp"
#include "hrtim.h"
#include <stm32f3xx.h>

extern bool callback_start;

void main_loop()
{
    init_common();
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA1 | HRTIM_OUTPUT_TA2);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_A);
    printf("Hello\n");

    callback_start = true;

    while (1) {
    }
}

void cycle_callback()
{
    static uint32_t counter = 0;
    ++counter;
    if (counter % 1000 < 500) {
        HAL_HRTIM_Set_Compare(&hhrtim1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_OUTPUT_TA1, 10000);
    } else {
        HAL_HRTIM_Set_Compare(&hhrtim1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_OUTPUT_TA1, 20000);
    }
}
