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

uint32_t get_systick_value()
{
    return SysTick->VAL;
}

// 新旧のSysTickの値から経過したクロックを数える
// SysTickの値はカウントダウンされていくことに注意
inline uint32_t calc_elapsed_clock(uint32_t new_systick, uint32_t old_systick)
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
void delay_ms(uint32_t ms)
{
    delay_us(1000 * ms);
}

/*!
 * \brief マイクロ秒単位のディレイ
 *
 * \param[in] us マイクロ秒単位の待ち時間
 */
void delay_us(uint32_t us)
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

/**
 * @brief This function handles HRTIM timer A global interrupt.
 */
void HRTIM1_TIMA_IRQHandler(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    // GPIOB->BSRR = GPIO_PIN_1;
    HAL_HRTIM_IRQHandler(&hhrtim1, HRTIM_TIMERINDEX_TIMER_A);
    // __HAL_HRTIM_TIMER_CLEAR_IT(&hhrtim1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_TIM_IT_UPD);
    // GPIOB->BRR = GPIO_PIN_1;
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    // HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
}

}  // extern "C"
