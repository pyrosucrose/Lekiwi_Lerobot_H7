//
// Created by Glucose_carbide on 25-8-8.
//

#pragma once

// =============================== 调用库 ===============================
#include <cstdint>
#include "math.tpp"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ===============================
inline constexpr float USER_PI = 3.14159265f;
inline constexpr float USER_2PI = 2.0f * 3.14159265f;
inline constexpr float USER_1_PI = 1.0f / USER_PI;
inline constexpr float USER_1_2PI = 1.0f / (2.0f * USER_PI);

inline constexpr float DEG_TO_RAD = USER_PI / 180.0f;
inline constexpr float RAD_TO_DEG = 180.0f / USER_PI;
inline constexpr float RPM_TO_RAD = USER_PI / 30.0f;
inline constexpr float RAD_TO_RPM = 30.0f * USER_1_PI;

// =============================== 函数声明 ===============================
[[nodiscard]] float AbsMax(const float *list, uint8_t len = 4);
[[nodiscard]] float Max(const float *list, uint8_t len = 4);
[[nodiscard]] float Min(const float *list, uint8_t len = 4);
bool Round_p(float* p_value, float limit = 360.0f);
[[nodiscard]] float Round_v(float value, float limit = 360.0f);
bool Limit(float* value, float min, float max);

// =============================== 库函数实现 ===============================

/// 角度值转为弧度值
[[nodiscard]] constexpr float Degree2Rad(const float angle)
{
    return angle * DEG_TO_RAD;
}

/// 弧度值转为角度值
[[nodiscard]] constexpr float Rad2Degree(const float rad)
{
    return rad * RAD_TO_DEG;
}

/// rad变为圈数
[[nodiscard]] constexpr float Rad2Round(const float rad)
{
    return rad * USER_1_2PI;
}

[[nodiscard]] constexpr float Round2Rad(const float round)
{
    return round * USER_2PI;
}

/// RPM变为rad/s
[[nodiscard]] constexpr float Rpm2Rad(const float rpm)
{
    return rpm * RPM_TO_RAD;
}

/// rad/s变为RPM
[[nodiscard]] constexpr float Rad2Rpm(const float rad)
{
    return rad * RAD_TO_RPM;
}

/// 步进，将ramp_last以ramp_step的最大步长向target步进
[[nodiscard]] constexpr float Ramp(const float target, const float ramp_last, const float ramp_step)
{
    return fabsf(target - ramp_last) <= ramp_step ? target : ramp_last + ramp_step * fabsf(target - ramp_last) / (target - ramp_last);
}

/// 线性插值，求经过(x1, y1), (x2, y2)的直线在value处的y值
[[nodiscard]] constexpr float Map(const float value, const float x1, const float x2, const float y1, const float y2)
{
    return (value - x1) * (y2 - y1) / (x2 - x1) + y1;
}
