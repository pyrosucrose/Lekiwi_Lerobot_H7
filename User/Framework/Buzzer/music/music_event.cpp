//
// Created by Glucose_carbide on 2026-04-15.
//

#include "music_event.h"

#include "usartio.hpp"

namespace Buzzer
{
    [[nodiscard]] constexpr uint16_t Bpm2MsPerBeat(const uint16_t bpm)
    {
        if (bpm == 0)
            return 100;
        return 60000 / bpm;
    }

    void MusicEvent::update(const uint8_t period)
    {
        if (track_ == nullptr || trig_ == nullptr || track_len_ <= 1)
        {   // safety check
            ms_per_beat_ = 1000;
            idx_ = 0;
            note_left_ms_ = 0;
            gate_left_ms_ = 0;
            hz_ = 2000;
            duty_ = 50;
            return;
        }

        // 如果遇到配置单元，就配置BPM，直到遇到note单元
        bool found_note = false;
        for (uint16_t guard = 0; guard < track_len_; guard++)
        {
            if (const auto& unit = track_[idx_]; unit.IsBpm())
            {
                ms_per_beat_ = Bpm2MsPerBeat(unit.GetPitch());
                note_update_ = true;
                note_left_ms_ = 0; // 更新标志位，表示
                idx_++; idx_ %= track_len_;
                continue;
            }

            found_note = true;
            break;
        }

        if (!found_note)
        {   // safety check
            hz_ = 0;
            duty_ = 0;
            note_left_ms_ = 0;
            gate_left_ms_ = 0;
            return;
        }

        // 如果遇到note单元，则播放
        // const auto& [pitch, beats, on_percent, duty_percent] = track_[idx_].getNote();
        const uint16_t pitch = track_[idx_].GetPitch();
        const uint8_t beats = track_[idx_].GetBeats();
        const uint8_t on_percent = track_[idx_].GetOnRatio() * 100 / 0xF;
        const uint8_t duty = track_[idx_].GetDuty() * 100 / 0xF;

        // 若单元更新
        if (note_update_)
        {
            note_left_ms_ = beats == 0 ? 1 : beats * ms_per_beat_;
            gate_left_ms_ = note_left_ms_ * on_percent / 100;
            note_update_ = false;
        }

        hz_ = pitch;
        duty_ = gate_left_ms_ > 0 ? duty : 0;

        // 计数与单元结束处理
        note_left_ms_ -= period;
        gate_left_ms_ -= period;

        if (note_left_ms_ <= 0)
        {
            note_update_ = true;
            note_left_ms_ = 0;
            gate_left_ms_ = 0;
            idx_++; idx_ %= track_len_;
        }
        // usart_printf("%d,%d,%d,%d\n", hz_, duty_, note_left_ms_, idx_);
    }

    void MusicEvent::reset()
    {
        hz_ = 2000;
        duty_ = 50;
        note_update_ = true;
        idx_ = 0;
        note_left_ms_ = 0;
        gate_left_ms_ = 0;
        ms_per_beat_ = 1000;
    }

    void MusicEvent::reload()
    {
        if (restart_on_interrupt_)
            reset();
    }
} // namespace ega::Buzzer
