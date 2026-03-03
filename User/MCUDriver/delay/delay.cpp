//
// Created by Glucose_carbide on 25-8-8.
//

// =============================== 引入头文件 ===============================
#include "delay.hpp"
#include "stm32h7xx_hal.h"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ==================================
static uint8_t miu_seconds_factor   = SystemCoreClock / 1000000;
static uint32_t milliseconds_factor = SystemCoreClock / 1000;

// =============================== 函数实现 ===============================

/**
  * @brief  微秒级延时
  * @param  miu_seconds: 要延时的微秒数
  * @note   使用SysTick定时器实现精确微秒延时
  */
void Delay_us(uint16_t miu_seconds)
{
    uint32_t target_ticks = 0;
    uint32_t time_previous = 0;
    uint32_t time_current = 0;
    uint32_t elapsed_ticks = 0;
    uint32_t reload_value = 0;

    reload_value = SysTick->LOAD;
    target_ticks = miu_seconds * miu_seconds_factor;
    time_previous = SysTick->VAL;

    while (true)
    {
        time_current = SysTick->VAL;
        if (time_current != time_previous)
        {
            if (time_current < time_previous)
            {
                elapsed_ticks += time_previous - time_current;
            }
            else
            {
                elapsed_ticks += reload_value - time_current + time_previous;
            }
            time_previous = time_current;
            if (elapsed_ticks >= target_ticks)
            {
                break;
            }
        }
    }
}

/**
  * @brief  毫秒级延时
  * @param  milliseconds: 要延时的毫秒数
  * @note   使用SysTick定时器实现精确毫秒延时
  */
void delay_ms(uint16_t milliseconds)
{
    uint32_t target_ticks = 0;
    uint32_t time_previous = 0;
    uint32_t time_current = 0;
    uint32_t elapsed_ticks = 0;
    uint32_t reload_value = 0;

    reload_value = SysTick->LOAD;
    target_ticks = milliseconds * milliseconds_factor;
    time_previous = SysTick->VAL;

    while (true)
    {
        time_current = SysTick->VAL;
        if (time_current != time_previous)
        {
            if (time_current < time_previous)
            {
                elapsed_ticks += time_previous - time_current;
            }
            else
            {
                elapsed_ticks += reload_value - time_current + time_previous;
            }
            time_previous = time_current;
            if (elapsed_ticks >= target_ticks)
            {
                break;
            }
        }
    }
}