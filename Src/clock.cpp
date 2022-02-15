#include <stm32f3xx.h>
#include "clock.hpp"
#include <stdio.h>
#include "utils.hpp"
#include "main_cycle.hpp"

extern uint32_t SystemCoreClock;
uint32_t systick_max_value;

void clock_init()
{
    SystemCoreClockUpdate();
    systick_max_value = SystemCoreClock / 1000;

    HAL_SYSTICK_Config(systick_max_value);
    HAL_NVIC_SetPriority(SysTick_IRQn, 4, 0);
}

std::uint32_t get_systick_value()
{
    return SysTick->VAL;
}

// 新旧のSysTickの値から経過したクロックを数える
// SysTickの値はカウントダウンされていくことに注意
inline std::uint32_t calc_elapsed_clock(std::uint32_t new_systick, std::uint32_t old_systick)
{
    if (old_systick > new_systick) {
        return old_systick - new_systick;
    } else {
        return old_systick + systick_max_value - new_systick;
    }
}

/*!
 * \brief ミリ秒単位のディレイ
 *
 * \param[in] ms ミリ秒単位の待ち時間
 */
void delay_ms(std::uint32_t ms)
{
    delay_us(1000 * ms);
}

/*!
 * \brief マイクロ秒単位のディレイ
 *
 * \param[in] us マイクロ秒単位の待ち時間
 */
void delay_us(std::uint32_t us)
{
    uint32_t old_clock = get_systick_value();
    uint32_t delay_clock = us * (SystemCoreClock / 1000 / 1000);
    uint32_t lapse_clock = 0;
    while (lapse_clock < delay_clock) {
        uint32_t new_clock = get_systick_value();
        uint32_t diff_clock = calc_elapsed_clock(new_clock, old_clock);
        lapse_clock += diff_clock;
        old_clock = new_clock;
    }
}

extern "C" {

bool callback_start = false;
void SysTick_Handler()
{
    if (!callback_start) {
        return;
    }
    cycle_callback();
    HAL_IncTick();
}
}  // extern "C"
