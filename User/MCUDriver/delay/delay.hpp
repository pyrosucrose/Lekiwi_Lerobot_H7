//
// Created by Glucose_carbide on 25-8-8.
//

#pragma once

// =============================== 调用库 ===============================
#include <cstdint>
#include "stm32h7xx_hal.h"
#include "usartio.hpp"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ===============================
inline const uint8_t miu_seconds_factor   = SystemCoreClock / 1000000;
inline const uint32_t milliseconds_factor = SystemCoreClock / 1000;

// =============================== 函数声明 ===============================
void Delay_us(uint16_t miu_seconds);
void Delay_ms(uint16_t milliseconds);

inline uint32_t GetTimeStamp()
{
    return SysTick->VAL;
}

inline float CalculateInterval(uint32_t start, uint32_t end)
{
    start &= 0x00FFFFFF;
    end &= 0x00FFFFFF;

    const uint32_t delta_ticks = start - end;
    // const uint32_t delta_ticks = start >= end ? start - end : (0x01000000 - end) + start;
    // return static_cast<float>(delta_ticks) / 480;
    return static_cast<float>(delta_ticks) / static_cast<float>(miu_seconds_factor);
}

// =============================== 类声明 ===============================
