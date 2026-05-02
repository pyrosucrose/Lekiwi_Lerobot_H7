//
// Created by Glucose_carbide on 2026-05-02.
//

#include "math.hpp"

/**
 * @brief   传入一个数组，找出其中绝对值最大的元素并返回该绝对值
 * @param   list: 传入的四元数组指针
 * @param   len: 传入的数组长度，默认为4
 * @note    必须处理返回值！
 * @retval  绝对值最大的元素
 */
[[nodiscard]] float AbsMax(const float *list, const uint8_t len)
{
    float max = fabsf(list[0]);
    for (int i = 0; i < len; i++)
        if (fabsf(list[i]) > max)
            max = fabsf(list[i]);
    return max;
}

/**
 * @brief   传入一个数组，找出其中最大的元素并返回该数
 * @param   list: 传入的四元数组指针
 * @param   len: 传入的数组长度，默认为4
 * @note    必须处理返回值！
 * @retval  最大的元素
 */
[[nodiscard]] float Max(const float *list, const uint8_t len)
{
    float max = list[0];
    for (int i = 0; i < len; i++)
        if (list[i] > max)
            max = list[i];
    return max;
}

/**
 * @brief   传入一个数组，找出其中最小的元素并返回该数
 * @param   list: 传入的四元数组指针
 * @param   len: 传入的数组长度，默认为4
 * @note    必须处理返回值！
 * @retval  最小的元素
 */
[[nodiscard]] float Min(const float *list, const uint8_t len)
{
    float min = list[0];
    for (int i = 0; i < len; i++)
        if (list[i] < min)
            min = list[i];
    return min;
}

/**
 * @brief   将指针指向的数据(通常是角度)滚到[-limit/2, limit/2]间
 * @param   p_value: 待处理的角度指针
 * @param   limit: 限幅范围，会将输入的角度滚在[-limit/2, limit/2]间
 * @note    是将数据累加/累减到范围中(类似int溢出)，而不是钳制
 * @note    务必确认传入的单位正确！
 * @retval  是否改变了数据
 */
bool Round_p(float* p_value, const float limit)
{
    bool state = false;
    while (*p_value > limit / 2)
    {
        *p_value -= limit;
        state = true;
    }
    while (*p_value < -limit / 2)
    {
        *p_value += limit;
        state = true;
    }
    return state;
}

/**
 * @brief   将传入的数据(通常是角度)滚到[-limit/2, limit/2]间并返回
 * @param   value: 待处理的角度值
 * @param   limit: 限幅范围，会将输入的角度滚在[-limit/2, limit/2]间
 * @note    是将数据累加/累减到范围中(类似int溢出)，而不是钳制
 * @note    务必确认传入的单位正确！
 * @retval  滚后的数据
 */
float Round_v(float value, const float limit)
{
    while (value > limit / 2)
        value -= limit;
    while (value < -limit / 2)
        value += limit;
    return value;
}

/**
 * @brief   将输入的数值限制在[min, max]间并返回
 * @param   value: 待处理的角度数据
 * @param   min: 最小值
 * @param   max: 最大值
 * @retval  是否改变了数据
 * @todo    屎山！屎山！屎山！屎山！屎山！屎山！屎山！屎山！屎山！屎山！屎山！屎山！屎山！
 */
bool Limit(float* value, const float min, const float max)
{
    if (*value > max)
    {
        *value = max;
        return true;
    }
    if (*value < min)
    {
        *value = min;
        return true;
    }
    return false;
}

