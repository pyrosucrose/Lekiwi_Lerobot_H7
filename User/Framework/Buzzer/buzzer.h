//
// Created by Glucose_carbide on 2026-4-15.
//

#ifndef BUZZER_HPP
#define BUZZER_HPP

#include <cstdint>

#include "music_event.h"

namespace Buzzer
{
    void Init();
    void SingBlock(uint16_t pitch, uint16_t duration_ms);
    bool AddToNoteTrack(uint16_t pitch, uint16_t duration_ms);
    bool AddConfig(const Config& config);
    void UpdateTriggers();
    void ControlLoop(uint8_t period);
}

#endif

