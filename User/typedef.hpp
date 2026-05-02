//
// Created by Glucose_carbide on 2026-05-02.
//

#pragma once

#include <cstdint>

#define FSI6X   1
#define DT7     2

/// 遥控器拨杆
typedef enum
{
    HIGH = 1,
    MIDDLE = 3,
    LOW = 2
} eRemoteSwitchValue;

/// 底盘坐标系
typedef enum : uint8_t
{
    X = 0,
    Y = 1,
    Z = 2
} eChassisAxis;
