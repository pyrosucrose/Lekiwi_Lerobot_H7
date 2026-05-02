//
// Created by Glucose_carbide on 2026-05-02.
//

#include "tim.hpp"

void cTim::Init() const
{
    HAL_TIM_Base_Start(htim);
    HAL_TIM_PWM_Start(htim, tim_channel);
}

void cTim::Mute() const
{
    __HAL_TIM_SetCompare(htim, tim_channel, 0);
}

void cTim::ChangeFreqDuty(const uint16_t freq, const uint8_t duty) const
{
    if (freq >= 16)
    {
        const uint16_t arr = 1000000 / freq;
        __HAL_TIM_SetAutoreload(htim, arr);
        // htim->Instance->ARR = arr;
        ChangeDuty(duty);
    }
    else
        Mute();
}

void cTim::ChangeDuty(const uint8_t duty) const
{
    const uint16_t arr = htim->Instance->ARR;
    const uint16_t ccr = arr * duty / 100;
    __HAL_TIM_SetCompare(htim, tim_channel, ccr);
}
