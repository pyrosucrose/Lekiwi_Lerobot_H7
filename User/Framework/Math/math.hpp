//
// Created by Glucose_carbide on 25-8-8.
//

#pragma once

// =============================== 调用库 ===============================
#include <cstdint>
#include "math.tpp"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ===============================

// =============================== 函数声明 ===============================
[[nodiscard]] float AbsMax_List(const float *list, uint8_t len = 4);
[[nodiscard]] float Max_List(const float *list, uint8_t len = 4);
[[nodiscard]] float Min_List(const float *list, uint8_t len = 4);
bool Round_p(float* p_value, float limit = 360.0f);
[[nodiscard]] float Round_v(float value, float limit = 360.0f);
bool Limit(float* value, float min, float max);

// =============================== 库函数实现 ===============================

/// 角度值转为弧度值
[[nodiscard]] inline float Degree2Rad(const float angle)
{
    return static_cast<float>(angle * M_PI / 180.0f);
}

/// 弧度值转为角度值
[[nodiscard]] inline float Rad2Degree(const float rad)
{
    return static_cast<float>(rad * 180.0f / M_PI);
}

/// RPM变为rad/s
[[nodiscard]] inline constexpr float Rpm2Rad(const float rpm)
{
    return static_cast<float>(rpm * 2 * M_PI / 60);
}

/// rad/s变为RPM
[[nodiscard]] inline constexpr float Rad2Rpm(const float rad)
{
    return static_cast<float>(rad * 60.0f / (M_PI * 2));
}

/// 限幅
[[nodiscard]] inline float Clamp(const float value, const float min, const float max)
{
    return std::max(min, std::min(value, max));
}

/// 步进，将ramp_last以ramp_step的最大步长向target步进
[[nodiscard]] inline float Ramp(const float target, const float ramp_last, const float ramp_step)
{
    return fabsf(target - ramp_last) <= ramp_step ? target : ramp_last + ramp_step * fabsf(target - ramp_last) / (target - ramp_last);
}

/// 线性插值，求经过(x1, y1), (x2, y2)的直线在value处的y值
[[nodiscard]] inline float Map(const float value, const float x1, const float x2, const float y1, const float y2)
{
    return (value - x1) * (y2 - y1) / (x2 - x1) + y1;
}

/// target是否在num1和num2之间，区间会往两边扩tolerance以适配浮点误差
[[nodiscard]] inline bool IsBetween(const float target, const float num1, const float num2, const float tolerance = 1e-6f)
{
    return (target >= std::min(num1, num2) - tolerance) && (target <= std::max(num1, num2) + tolerance);
}
