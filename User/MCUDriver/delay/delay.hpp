//
// Created by Glucose_carbide on 25-8-8.
//

#pragma once

// =============================== 调用库 ===============================
#include "tim.h"
namespace Delay
{
    // =============================== 宏定义区 ===============================
    inline constexpr TIM_HandleTypeDef* delay_handle = &htim13;
    inline constexpr uint8_t FREQ_MHZ = 1;
    inline constexpr uint16_t MAX_SAFE_TICK = 64000;

    inline constexpr float US_FACTOR = 1.0f / FREQ_MHZ;
    inline constexpr uint32_t MAX_SAFE_US = MAX_SAFE_TICK * US_FACTOR;
    inline constexpr uint32_t MAX_SAFE_MS = MAX_SAFE_US / 1000 - 1;

    // =============================== 变量区 ===============================

    // =============================== 函数声明 ===============================
    inline void Init()
    {
        HAL_TIM_Base_Start(delay_handle);
    }

    inline uint16_t GetTimeStamp()
    {
        return delay_handle->Instance->CNT;
    }

    inline float CalculateInterval_us(const uint16_t start)
    {
        const auto now = GetTimeStamp();
        const uint16_t ticks = now - start;

        return US_FACTOR * static_cast<float>(ticks);
    }

    inline void Delay_us(const uint32_t us)
    {
        if (us == 0) return;

        uint32_t wait_ticks = us * FREQ_MHZ;
        while (wait_ticks > MAX_SAFE_TICK)
        {
            wait_ticks -= MAX_SAFE_TICK;
            const auto start = GetTimeStamp();
            while (GetTimeStamp() - start < MAX_SAFE_TICK);
        }

        const auto start = GetTimeStamp();
        while (GetTimeStamp() - start < wait_ticks);
    }

    inline void Delay_ms(uint32_t ms)
    {
        while (ms > MAX_SAFE_MS)
        {
            ms -= MAX_SAFE_MS;
            Delay_us(MAX_SAFE_MS * 1000);
        }
        Delay_us(ms * 1000);
    }
}
// =============================== 类声明 ===============================
