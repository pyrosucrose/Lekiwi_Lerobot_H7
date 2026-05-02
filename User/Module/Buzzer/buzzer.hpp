//
// Created by Glucose_carbide on 2026-04-15.
//

#pragma once

#include <cstdint>

#include "music_event.hpp"

namespace Buzzer
{
    void Init();
    void SetTone(uint16_t freq, uint8_t duty = 50);
    void SingBlock(uint16_t pitch, uint16_t duration_ms);
    bool AddToNoteTrack(uint16_t pitch, uint16_t duration_ms);
    bool AddConfig(const Config& config);
    void UpdateTriggers();
    void ControlLoop();
}

