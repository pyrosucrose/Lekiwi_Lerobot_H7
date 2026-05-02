//
// Created by Glucose_carbide on 2026-05-02.
//

#pragma once

#include "tim.h"
namespace Delay
{
    inline constexpr TIM_HandleTypeDef* DELAY_HANDLE = &htim13;
    inline constexpr uint8_t FREQ_MHZ = 1;
    inline constexpr uint16_t MAX_SAFE_TICK = 60000;

    inline constexpr float US_FACTOR = 1.0f / FREQ_MHZ;
    inline constexpr uint32_t MAX_SAFE_US = MAX_SAFE_TICK * US_FACTOR;
    inline constexpr uint32_t MAX_SAFE_MS = MAX_SAFE_US / 1000 - 1;

    /**
     * @brief   初始化延时模块
     * @note    启动底层硬件定时器，必须在调用其他延时函数前调用此函数
     */
    inline void Init()
    {
        HAL_TIM_Base_Start(DELAY_HANDLE);
    }

    /**
     * @brief   获取当前时间戳
     * @retval  定时器当前的计数器值 (CNT)
     * @note    虽然 CNT 是32位，但实际上有效值只有16位
     */
    inline uint16_t GetTimeStamp()
    {
        return DELAY_HANDLE->Instance->CNT;
    }

    /**
     * @brief   计算两个时间点之间的时间间隔
     * @param   s: 起始时间戳 (由 GetTimeStamp 获取)
     * @retval  经过的时间 (单位: 微秒)
     * @warning 本函数依赖 16 位计数器差值计算，受限于硬件溢出特性。
     *          请勿计算超过 60/FREQ_MHZ 毫秒的时间间隔 (当前配置下约为 60 毫秒)，
     *          否则会导致计数器回绕，计算结果错误。
     */
    inline float CalculateInterval_us(const uint16_t s)
    {
        const auto now = GetTimeStamp();
        const uint16_t ticks = now - s;

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

    /**
     * @brief   维持固定周期的延时 (精准周期控制)
     * @param   s: 指向时间戳变量的指针，用于记录上一次循环的时间
     * @param   us: 期望的循环周期 (单位: 微秒)
     * @note    类似FreeRTOS的 vTaskDelayUntil，但由于是微秒级控制，不得不是阻塞式
     * @note    此函数会自动补偿代码执行时间，确保循环周期稳定。
     * @warning 请勿设置过长的周期，确保 us * FREQ_MHZ < 60000，
     *          即周期不超过 60/FREQ_MHZ 毫秒 (当前配置下约为 60 毫秒)。
     *          否则因计数器溢出导致延时逻辑失效。
     */
    inline void DelayUntil_us(uint16_t* const s, const uint16_t us)
    {
        if (!s) return;

        const auto start = GetTimeStamp();
        const uint16_t wait = *s - start + us * FREQ_MHZ;

        while (static_cast<uint16_t>(GetTimeStamp() - start) < wait) {}

        *s = GetTimeStamp();
    }

    /**
     * @brief   毫秒级阻塞延时
     * @param   ms: 需要延时的毫秒数
     * @note    基于 Delay_us 封装，用于实现长延时。
     *          内部已处理了 MAX_SAFE_MS 的限制，可安全传入任意毫秒数。
     */
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
