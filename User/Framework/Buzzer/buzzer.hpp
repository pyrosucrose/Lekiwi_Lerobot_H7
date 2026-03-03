//
// Created by Glucose_carbide on 25-7-23.
//

#ifndef BUZZER_HPP
#define BUZZER_HPP

// =============================== 调用库 ===============================
#include <cstdint>
#include "buzzer_soundtrack.hpp"
#include "tim.h"

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

class cBuzzer : public cTim
{
public:
    cBuzzer(TIM_HandleTypeDef* htim_, const uint32_t tim_channel_) :
        cTim(htim_, tim_channel_) {}

    void SelectSoundtrack();
    void UpdateSoundtrack();
    void ControlLoop();
private:
    eSoundtrack soundtrack_now = eSoundtrack::MUTE;
    eSoundtrack soundtrack_last = eSoundtrack::MUTE;
    uint32_t state = 0;
    uint32_t duration_frames = 0;
    uint32_t on_frames = 0;
    uint32_t frame_counter = 0;
    uint16_t track_length = 0;
    uint16_t note_index = 0;
    uint16_t frame_per_beat = 0;
    cSoundTrackUnit* soundtrack = nullptr;
};

#endif // BUZZER_HPP
