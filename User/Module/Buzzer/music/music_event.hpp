//
// Created by Glucose_carbide on 2026-04-15.
//

#pragma once

#include <cstdint>

#include "music_unit.hpp"

namespace Buzzer
{
    struct Config
    {
        const MusicUnit* soundtrack = nullptr;
        uint16_t track_len = 1;
        uint8_t priority = 0;
        bool* trigger = nullptr;
        bool active_trigger_state = true;
        bool restart_when_interrupted = false;
    };

    class MusicEvent
    {
    public:
        MusicEvent() = default;

        explicit MusicEvent(const Config& config) :
            track_(config.soundtrack),
            track_len_(config.track_len),
            prio_(config.priority),
            trig_(config.trigger),
            trig_active_(config.active_trigger_state),
            restart_on_interrupt_(config.restart_when_interrupted)
        {
        }

        [[nodiscard]] bool isTriggered() const { return trig_ ? *trig_ == trig_active_ : false; }
        [[nodiscard]] uint8_t getPriority() const { return prio_; }
        [[nodiscard]] uint16_t getPitch() const { return hz_; }
        [[nodiscard]] uint8_t getDuty() const { return duty_; }

        void update();
        void reload();
        void reset();

    private:
        const MusicUnit* track_ = nullptr;
        uint16_t track_len_ = 1;
        uint8_t prio_ = 0;
        bool* trig_ = nullptr;
        bool trig_active_ = true;
        bool restart_on_interrupt_ = false;

        uint16_t hz_ = 2000;
        uint8_t duty_ = 50;

        bool note_update_ = true;
        uint16_t idx_ = 0;
        int32_t note_left_ms_ = 0;
        int32_t gate_left_ms_ = 0;
        uint16_t ms_per_beat_ = 1000;
    };
}
