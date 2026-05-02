//
// Created by Glucose_carbide on 2026-04-24.
//

#pragma once

#include "music_unit.hpp"
#include "music_pitch.hpp"

namespace Buzzer::Soundtracks
{
    constexpr MusicUnit track_mute[] = {
        {440,0,0},
        {mute, 100},
        {mute, 100}
    };
}
