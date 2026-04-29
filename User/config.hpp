//
// Created by Glucose_carbide on 25-8-8.
//

#pragma once

// =============================== 调用库 ===============================
#include <cstdint>
#include <cmath>
#include "typedef.hpp"

/* ========== Configures the hardware parameters of the vehicle ========== */
/** Choose From {FSI6X, DT7} */
/* 遥控器类型选择 */
#define REMOTE_TYPE     FSI6X

/* ========== Configures the software parameters of the vehicle ========== */
/** Set The Values */
/* FreeRTOS中，每个task的周期(毫秒) */
inline constexpr uint8_t TASK_BUZZER_TASK_PERIOD     = 10;
inline constexpr uint8_t TASK_MOTOR_TASK_PERIOD      = 5;
inline constexpr uint8_t TASK_ROBOT_TASK_PERIOD      = 5;
inline constexpr uint8_t TASK_ROBOT_SEND_TASK_PERIOD = 3;
inline constexpr uint8_t TASK_DEBUG_TASK_PERIOD      = 4;
inline constexpr uint8_t TASK_LED_TASK_PERIOD        = 5;

// =============================== 参数计算区(不得修改) ===============================

