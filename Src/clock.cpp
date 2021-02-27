#include <stm32f3xx.h>
#include "clock.hpp"
#include <stdio.h>
#include "utils.hpp"
#include "main_cycle.hpp"

extern std::uint32_t SystemCoreClock;
std::uint32_t SYSTICK_MAX_VALUE;
std::uint32_t g_system_clock_in_kHz;
std::uint32_t g_system_clock_in_MHz;

void clock_init()
{
    SystemCoreClockUpdate();
    g_system_clock_in_kHz = SystemCoreClock / 1000;
    g_system_clock_in_MHz = SystemCoreClock / 1000000;
    SYSTICK_MAX_VALUE = SystemCoreClock / 1000;  //callbackは1000Hz

    //systick_init
    HAL_SYSTICK_Config(SYSTICK_MAX_VALUE);
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
        return old_systick + SYSTICK_MAX_VALUE - new_systick;
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
    std::uint32_t old_clock = get_systick_value();
    std::uint32_t delay_clock = us * g_system_clock_in_MHz;
    std::uint32_t lapse_clock = 0;
    while (lapse_clock < delay_clock) {
        std::uint32_t new_clock = get_systick_value();
        std::uint32_t diff_clock = calc_elapsed_clock(new_clock, old_clock);
        lapse_clock += diff_clock;
        old_clock = new_clock;
    }
}

extern "C" {

bool callback_start = 0;

void SysTick_Handler()
{
    const std::uint32_t INITIALIZE_WAIT_CYCLE = 10;
    static std::uint32_t g_cycle_counter = 0;
    if (callback_start) {
        ++g_cycle_counter;
    }

    // 最初のn周期は処理を行わない
    if (g_cycle_counter <= INITIALIZE_WAIT_CYCLE) {
        return;
    }
    cycle_callback();
}
}  // extern "C"
