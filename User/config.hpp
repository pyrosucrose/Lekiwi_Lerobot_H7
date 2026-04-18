//
// Created by Glucose_carbide on 25-8-8.
//

#ifndef CONFIG_HPP
#define CONFIG_HPP

// =============================== 调用库 ===============================
#include <cstdint>
#include <cmath>
#include "typedef.hpp"

/* ========== Configures the hardware parameters of the vehicle ========== */
/** Choose From {FSI6X, DT7} */
/* 遥控器类型选择 */
#define REMOTE_TYPE     FSI6X

/** Choose From {MECANUM, OMNI, HELM} */
/* 底盘类型选择 */
#define CHASSIS_TYPE    HELM

/** Set The Values */
/* 遥控器->底盘参数设置 */
inline constexpr float CHASSIS_TRANSLATION_SPEED_MAX = 1.0f;    // 平移xy轴速度分量最大值(m/s)
inline constexpr float CHASSIS_ROTATION_SPEED_MAX    = 1.0f;   // 旋转最大角速度(rad/s)
inline constexpr float CHASSIS_RAMP_STEP_TRANSLATION = 0.03f;   // 底盘平动速度步进值
inline constexpr float CHASSIS_RAMP_STEP_ROTATION    = 0.06f;   // 底盘旋转角速度步进值

/** Set The Values */
/* 底盘参数设置 */
inline constexpr float CHASSIS_WHEEL_RADIUS          = 0.08f;   // 底盘轮子的半径
inline constexpr float CHASSIS_HALF_LENGTH           = 0.175f;  // 四个轮子形成的矩形的前进方向(一般为长边方向)
inline constexpr float CHASSIS_HALF_WIDTH            = 0.175f;  // 四个轮子形成的矩形的左右方向(一般为短边方向)

/* ========== Configures the software parameters of the vehicle ========== */
/** Set The Values */
/* FreeRTOS中，每个task的周期(毫秒) */
inline constexpr uint8_t TASK_BMI_TASK_PERIOD        = 2;
inline constexpr uint8_t TASK_BUZZER_TASK_PERIOD     = 2;
inline constexpr uint8_t TASK_ROBOT_TASK_PERIOD      = 5;
inline constexpr uint8_t TASK_ROBOT_SEND_TASK_PERIOD = 2;
inline constexpr uint8_t TASK_DEBUG_TASK_PERIOD      = 4;
inline constexpr uint8_t TASK_LED_TASK_PERIOD        = 5;

// =============================== 参数计算区(不得修改) ===============================
inline const float CHASSIS_RADIUS   = sqrtf(CHASSIS_HALF_LENGTH * CHASSIS_HALF_LENGTH +\
                                            CHASSIS_HALF_WIDTH * CHASSIS_HALF_WIDTH);


#endif // CONFIG_HPP
