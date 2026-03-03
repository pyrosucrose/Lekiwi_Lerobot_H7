//
// Created by Glucose_carbide on 25-7-23.
//

// =============================== 引入头文件 ===============================
#include "buzzer.hpp"
#include "config.hpp"
#include "stm32h7xx_hal.h"
#include "tim.h"
#include "buzzer_soundtrack.hpp"
#include "usartio.hpp"

// =============================== 宏定义区 ===============================
static constexpr float UPDATE_FREQUENCY = 1000.0f / TASK_BUZZER_TASK_PERIOD;

// =============================== 变量区 ===============================

// =============================== 函数实现 ===============================
[[nodiscard]] static inline int BPM2Period(const uint16_t bpm)
{
    return static_cast<int>(60.0f * UPDATE_FREQUENCY / bpm);
}

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
    if (freq != 0)
    {
        uint16_t arr = 1000000 / freq;
        if (arr >= 65535)
            arr = 65535;
        // __HAL_TIM_SetAutoreload(htim, arr);
        htim->Instance->ARR = arr;
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

/// 根据当前状态选择音轨
void cBuzzer::SelectSoundtrack()
{
    soundtrack_last = soundtrack_now;
    switch (state)
    {
    default:
        soundtrack_now = eSoundtrack::MUTE;
        soundtrack = soundtrack_immortal_ash;
        track_length = sizeof(soundtrack_immortal_ash) / sizeof(soundtrack_immortal_ash[0]);
        break;
    }
}

void cBuzzer::UpdateSoundtrack()
{
    if (soundtrack_now != soundtrack_last)
    {
        soundtrack_last = soundtrack_now;
        note_index = 0;
        frame_counter = 0;
        frame_per_beat = 200;
    }

    while (soundtrack[note_index].GetPeriod() == 0)
    {
        frame_per_beat = BPM2Period(soundtrack[note_index].GetPitch());
        if (++note_index >= track_length)
            note_index = 0;
    }

    // usart_printf("%d\n",frame_per_beat);
    const uint16_t pitch = soundtrack[note_index].GetPitch();
    const uint8_t period = soundtrack[note_index].GetPeriod();
    const uint8_t on_ratio = soundtrack[note_index].GetOnRatio();
    const uint8_t duty = soundtrack[note_index].GetDuty() * 100 / 0x0F;

    duration_frames = static_cast<uint32_t>(period) * frame_per_beat;
    on_frames = (duration_frames * on_ratio) / 0x0F;

    if (duration_frames == 0)
        duration_frames = 1;

    if (frame_counter < on_frames)
        ChangeFreqDuty(pitch, duty);
    else
        Mute();

    if (++frame_counter >= duration_frames)
    {
        frame_counter = 0;
        if (++note_index >= track_length)
            note_index = 0;
    }
}

void cBuzzer::ControlLoop()
{
    SelectSoundtrack();
    UpdateSoundtrack();
}


// /// 根据当前音轨进行帧操作
// void cBuzzer::UpdateSoundtrack()
// {
//     if (soundtrack_now != soundtrack_last)
//     { // 若音轨变了，就重置参数
//         duration_frames = 100; // 防止用户忘配置导致除0等错误
//         on_frame = 100;
//         frame_counter = 0;
//         note_index = 0;
//         frame_per_beat = 200;
//     }
//
//     // 若遇到参数配置的单元，就配置，但必须移动到有控制发声的单元
//     bool is_freq_set = false;
//     do
//     {
//         if (soundtrack[note_index].GetPeriod() == 0)
//         {
//             frame_per_beat = BPM2Period(soundtrack[note_index].GetPitch());
//             frame_counter = 0;
//             if (++note_index >= track_length) note_index = 0;
//         }
//         else
//         {
//             uint16_t pitch = soundtrack[note_index].GetPitch();
//             uint16_t duty = soundtrack[note_index].GetOnRatio();
//             if (frame_counter >= on_frame) pitch = 0;
//             ChangeFreqDuty(pitch, duty);
//             is_freq_set = true;
//         }
//     } while (!is_freq_set);
//
//     // 前往下一帧
//     if (++frame_counter >= duration_frames)
//     {
//         if (++note_index >= track_length) note_index = 0;
//         frame_counter = 0;
//     }
// }
