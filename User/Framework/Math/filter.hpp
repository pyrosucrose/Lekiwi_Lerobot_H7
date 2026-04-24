//
// Created by Glucose_carbide on 25-8-8.
//

#pragma once

// =============================== 调用库 ===============================

// =============================== 宏定义区 ===============================

// =============================== 变量区 ===============================

// =============================== 函数声明 ===============================

// =============================== 类声明 ===============================

class LowPassFilter {
public:
    // 构造函数，默认alpha=0.3
    explicit LowPassFilter(float alpha_ = 0.3f) : alpha(alpha_) {}

    [[nodiscard]] float getValue() const { return last_value; }
    [[nodiscard]] float getAlpha() const { return alpha; }
    // 更新滤波值
    float update(float new_value) {
        if (!initialized) {
            // 第一次使用时直接使用原始值
            last_value = new_value;
            initialized = true;
            return new_value;
        }

        // 一阶低通滤波: y(n) = α * x(n) + (1-α) * y(n-1)
        last_value = alpha * new_value + (1.0f - alpha) * last_value;
        return last_value;
    }

    // 重置滤波器
    void reset() {
        initialized = false;
        last_value = 0;
    }

    // 设置滤波系数
    void setAlpha(float new_alpha) {
        alpha = new_alpha;
        if (alpha < 0.0f) alpha = 0.0f;
        if (alpha > 1.0f) alpha = 1.0f;
    }

private:
    float alpha;           // 滤波系数 (0-1)
    float last_value = 0;      // 上一次滤波后的值
    bool initialized = false;      // 是否已初始化
};
