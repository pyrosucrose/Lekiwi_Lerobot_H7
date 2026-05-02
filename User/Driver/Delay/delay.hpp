//
// Created by Glucose_carbide on 2026-05-02.
//

#pragma once

#include "tim.h"
namespace Delay
{
    inline constexpr TIM_HandleTypeDef* delay_handle = &htim13;
    inline constexpr uint8_t FREQ_MHZ = 1;
    inline constexpr uint16_t MAX_SAFE_TICK = 60000;

    inline constexpr float US_FACTOR = 1.0f / FREQ_MHZ;
    inline constexpr uint32_t MAX_SAFE_US = MAX_SAFE_TICK * US_FACTOR;
    inline constexpr uint32_t MAX_SAFE_MS = MAX_SAFE_US / 1000 - 1;

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

        uint32_t wait = us * FREQ_MHZ;
        while (wait > MAX_SAFE_TICK)
        {
            wait -= MAX_SAFE_TICK;
            const auto start = GetTimeStamp();
            while (static_cast<uint16_t>(GetTimeStamp() - start) < MAX_SAFE_TICK);
        }

        const auto start = GetTimeStamp();
        while (static_cast<uint16_t>(GetTimeStamp() - start) < wait);
    }

    inline void DelayUntil_us(uint16_t* const s, const uint16_t us)
    {
        if (!s) return;

        const auto start = GetTimeStamp();
        const uint16_t wait = *s - start + us * FREQ_MHZ;

        while (static_cast<uint16_t>(GetTimeStamp() - start) < wait) {}

        *s = GetTimeStamp();
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
