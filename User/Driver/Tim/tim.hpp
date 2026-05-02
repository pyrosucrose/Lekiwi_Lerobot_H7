//
// Created by Glucose_carbide on 2026-05-02.
//

#pragma once

#include "tim.h"

class cTim
{
public:
    cTim(TIM_HandleTypeDef* htim_, const uint32_t tim_channel_) :
        htim(htim_), tim_channel(tim_channel_) {}

    void Init() const;
    void Mute() const;
    void ChangeFreqDuty(uint16_t freq, uint8_t duty = 50) const;
    void ChangeDuty(uint8_t duty = 50) const;
private:
    TIM_HandleTypeDef* htim;
    uint32_t tim_channel;
};
