//
// Created by Glucose_carbide on 25-8-8.
//

#ifndef PID_HPP
#define PID_HPP

// =============================== 调用库 ===============================

// =============================== 宏定义区 ===============================


// =============================== 变量区 ===============================

// =============================== 函数声明 ===============================

// =============================== 类声明 ===============================
class cPid
{
public:
    cPid(float kp, float ki, float kd,
     float error_max, float error_sum_max,
     float output_max) :
        kp_(kp), ki_(ki), kd_(kd),
        error_max_(error_max), error_sum_max_(error_sum_max),
        output_max_(output_max)
    {}

    cPid(float kp, float ki, float kd,
         float error_max, float error_sum_max,
         float output_max, float output_step_max) :
            kp_(kp), ki_(ki), kd_(kd),
            error_max_(error_max), error_sum_max_(error_sum_max),
            output_max_(output_max), output_step_max_(output_step_max)
    {}

    void SetTarget(float target) {target_now_ = target;};
    float CalculateOutput(float input);
    void Clear();

    [[nodiscard]] float GetOutputNow() const { return output_now_; }
    [[nodiscard]] float GetTargetNow() const { return target_now_; }
    void SetOutput(float output_now) { output_now_ = output_now; }

private:
    float kp_ = 0;                      // 比例项系数
    float ki_ = 0;                      // 积分项系数
    float kd_ = 0;                      // 微分项系数

    float input_now_ = 0;               // 当前输入值,rpm
    float target_now_ = 0;              // 当前目标值,rpm
    float output_now_ = 0;              // 当前输出值,pid电流,A

    float error_sum_ = 0;               // 累计误差值
    float error_now_ = 0;               // 当前误差值
    float error_last_ = 0;              // 上次误差值
    float error_max_ = 0;               // 误差最大值，超出此值则限制在此值内，防止一次异常尖峰导致积分项爆炸
    float error_sum_max_ = 655350;      // 最大累计误差值

    float output_last_ = 0;             // 上次输出值
    float output_max_ = 10000;          // 最大输出值
    float output_step_max_ = 65535;     // 最大输出步进值

    float precision_ = 0;               // 精确范围
};
#endif // PID_HPP
