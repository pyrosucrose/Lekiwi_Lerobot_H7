//
// Created by Glucose_carbide on 2026-04-15.
//

#pragma once

#include "music_pitch.hpp"

namespace Buzzer
{
    class MusicUnit
    {
    public:
        /** 若period为0，则该单元用于配置bpm(用pitch参数)，亦即一分钟会有几个period */
        constexpr MusicUnit(const uint16_t pitch, const uint8_t beats = 1, const uint8_t on_ratio = 15, const uint8_t duty = 8)
        {
            data = static_cast<uint32_t>(pitch)  << 16 |
                   static_cast<uint32_t>(beats) << 8  |
                   (static_cast<uint32_t>(on_ratio) & 0x0F) << 4 |
                   static_cast<uint32_t>(duty) & 0x0F;
        }
        [[nodiscard]] bool IsBpm() const { return GetBeats() == 0; }
        [[nodiscard]] uint16_t GetPitch() const { return (data >> 16) & 0xFFFF; }   // 音高(Hz)
        [[nodiscard]] uint8_t GetBeats() const { return (data >> 8) & 0xFF; }      // 持续时间(beat)
        [[nodiscard]] uint8_t GetOnRatio() const { return (data >> 4) & 0x0F; }     // 鸣起时间(0~15 -> 0~100)
        [[nodiscard]] uint8_t GetDuty() const { return (data >> 0) & 0x0F; }        // PWM波占空比(0~15 -> 0~100)
    private:
        uint32_t data = 0x00FF0000;
    };

    constexpr MusicUnit bpm(const uint16_t bpm)
    {
        const MusicUnit unit{bpm,0,0,0};
        return unit;
    }

}
