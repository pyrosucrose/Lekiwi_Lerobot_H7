//
// Created by Glucose_carbide on 25-8-8.
//

// =============================== 引入头文件 ===============================
#include "pid.hpp"
#include "math.hpp"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ==================================

// =============================== 函数实现 ===============================

/**
 * @brief   PID计算，得到输出值
 * @param   input   PID目标值
 * @retval  output_now_ PID输出值
 */
float cPid::CalculateOutput(const float input)
{
    input_now_ = input;
    error_now_ = target_now_ - input_now_;
    Limit(&error_now_, -error_max_, error_max_);

    error_sum_ += error_now_;
    Limit(&error_sum_, -error_sum_max_, error_sum_max_);

    output_now_ = kp_ * error_now_ + ki_ * error_sum_ + kd_ * (error_now_ - error_last_);

    Limit(&output_now_, output_last_ - output_step_max_, output_last_ + output_step_max_);
    Limit(&output_now_, -output_max_, output_max_);

    output_last_ = output_now_;
    error_last_ = error_now_;

    return output_now_;
}

/**
 * @brief   清空PID参数
 * @retval  None
 */
void cPid::Clear()
{
    target_now_ = 0;
    input_now_ = 0;
    output_now_ = 0;
    output_last_ = 0;
    error_now_ = 0;
    error_sum_ = 0;
    error_last_ = 0;
}
