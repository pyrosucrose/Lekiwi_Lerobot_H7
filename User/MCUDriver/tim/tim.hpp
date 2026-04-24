//
// Created by Glucose_carbide on 25-8-8.
//

#pragma once

// =============================== 调用库 ===============================
#include "tim.h"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ===============================

// =============================== 函数声明 ===============================

// =============================== 类声明 ===============================

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
