//
// Created by Glucose_carbide on 25-8-8.
//

// =============================== 引入头文件 ===============================
#include "motor_sts.hpp"

#include "crash.hpp"
#include "delay.hpp"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ==================================

// =============================== 函数实现 ===============================
/**
 * @brief   拼接返回数据
 * @param   L: 低位数据(数组前)
 * @param   H: 高位数据(数组后)
 * @retval  拼接后数据，为int16
 */
int16_t MotorSts::PackStsData(const uint8_t L, const uint8_t H)
{
    auto s = static_cast<int16_t>(H << 8 | L);
    if ((s & 0x8000) != 0) s = static_cast<int16_t>(-(s & 0x7FFF));
    return s;
}

/**
 * @brief   处理STS特有的(呕)数据格式
 * @param   s: 传入的数据
 * @retval  转化为STS格式后的数据(bit15表示正负号，s & 0x7FFF是绝对值)
 * @note    通常只会对负数有影响
 * @note    虽然声明传入的是uint16，但传int16也是一样的
 * @note    想出这样处理(u)int16的家里清明节指定能多出些什么来
 */
uint16_t MotorSts::ConvertStsData(const uint16_t s)
{
    return s & 0x8000 ? static_cast<uint16_t>(-(s & 0x7FFF)) : s;
}

/**
 * @brief   表示传入的寄存器地址是否是某个16bit数据的低位寄存器
 * @param   reg:寄存器地址
 * @retval  是不是
 */
bool MotorSts::IsLowByteRegister(const REG reg)
{
    return reg == REG::TARGET_POSITION_L ||
           reg == REG::TARGET_SPEED_L ||
           reg == REG::MIN_ANGLE_LIMIT_L ||
           reg == REG::MAX_ANGLE_LIMIT_L ||
           reg == REG::MAX_TORQUE_L ||
           reg == REG::PROTECTION_CURRENT_L ||
           reg == REG::POSITION_CORRECTION_L ||
           reg == REG::MOVING_TIME_L ||
           reg == REG::TORQUE_LIMIT_L;
}

/**
 * @brief   电机的构造函数
 * @details 新建电机后会将其添加至静态数组中以便批量操作
 * @param   ID          :该电机ID
 * @param   zero_point  :电机零点(用户希望电机所在的零点)
 * @param   min_angle   :电机所有允许的姿态下，编码器最小值
 * @param   max_angle   :电机所有允许的姿态下，编码器最大值
 * @param   reversed    :是否反转，和安装方式相关(若用户期望的电机方向和点击实际方向相反就设为true)
 * @note    若从这进了Crash那可能是电机ID冲突或是注册的电机过多导致超出MAX_MOTORS_COUNT，需要修正或修改相关配置
 * @warning 对于重复ID的处理能力有限，用户必须保证没用重复ID的电机被写入！
 */
MotorSts::MotorSts(const uint8_t ID, const uint16_t zero_point, const uint16_t min_angle, const uint16_t max_angle, const bool reversed) :
    ID_(ID), is_reversed_(reversed), zero_point_ecd_(zero_point),
    min_pos_ecd_(min_angle), soft_min_pos_ecd_(static_cast<int16_t>(reversed ? zero_point - max_angle : min_angle - zero_point)),
    max_pos_ecd_(max_angle), soft_max_pos_ecd_(static_cast<int16_t>(reversed ? zero_point - min_angle : max_angle - zero_point))
{
    if (motors_count_ < MAX_MOTORS_COUNT)
    {
        motors_[motors_count_] = this;
        if (motors_idx_[ID] != 0x00 && motors_idx_[ID] != Special::ILLEGAL_ID)  // 没辙，否则idx_必须手动写255个0xFF进去
            // 你是不是注册了ID相同的电机 o_O
            Crash();
        motors_idx_[ID] = motors_count_;
        motors_count_++;
    }
    else
        // 电机数量上限太小了，调大 MAX_MOTORS_COUNT
        Crash();
}

/**
 * @brief   电机的析构函数
 * @details 会查找静态电机列表找到当前电机，随后在idx_与motors_中注销该电机并且用最后一个电机覆盖
 * @note    若从这进了Crash那大概率是代码逻辑问题，还请及时汇报，感激不尽！
 */
MotorSts::~MotorSts()
{
    for (uint8_t i = 0; i < motors_count_; i++)
    {
        if (motors_[i] == this)
        {
            motors_idx_[motors_[motors_count_ - 1]->ID_] = i;
            motors_idx_[ID_] = Special::ILLEGAL_ID;

            motors_[i] = motors_[motors_count_ - 1]; // 用最后一个覆盖
            motors_[motors_count_ - 1] = nullptr;

            motors_count_--;
            return;
        }
    }
    // We should never get here as each registered motor should have a pool
    Crash();
}
