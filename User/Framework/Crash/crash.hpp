//
// Created by Glucose_carbide on 2026-04-29.
//

#pragma once

#include "buzzer.hpp"
#include "main.h"

inline void Crash()
{
    Buzzer::SetTone(2000);
    Error_Handler();
}
