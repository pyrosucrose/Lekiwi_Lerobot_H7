//
// Created by Glucose_carbide on 2026-04-30.
//

// =============================== 引入头文件 ===============================
#include "motor_sts.hpp"

#include <cstring>

#include "config.hpp"
#include "crash.hpp"
#include "usartio.hpp"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ==================================

// =============================== 函数实现 ===============================
/**
 * @brief   电机的中断回调处理函数
 * @details 该函数会先比对数据格式的正确性，并且将有用数据拷贝进电机的rx_buffer_中
 * @param   data: 数据的指针
 * @note    该函数应该作为MotorSts::RxCallback的形式在串口接收回调中被调用，而不是在main或RTOS等地方被主动调用
 * @note    仅拷贝数据，解析数据需要用户主动调用MotorSts::UnpackAll
 * @note    如果从if中进入Crash，是由于电机buffer长度过短，可修改MAX_BUF_LEN配置，若是在函数结尾进入，大概率是代码逻辑问题，还请汇报！
 */
void MotorSts::RxCallback(const uint8_t* data)
{
    if (data[0] != 0xFF || data[1] != 0xFF) return;   // 数据错乱不处理

    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < data[3] + 3; i++)
        check_sum += data[i];
    check_sum = ~check_sum;
    if (data[data[3] + 3] != check_sum) return;     // 校验和不匹配不处理

    if (const uint8_t ID = data[2]; motors_[motors_idx_[ID]])
    {
        if (data[3] >= MAX_BUF_LEN)
            // 分配的BUF太小，调大 MAX_BUF_LEN
            Crash();
        if (data[3] - 2 > 0)
        {
            motors_[motors_idx_[ID]]->received_pack_ = true;        // 但仍需将标志位置1防止死循环 ↓
            if (motors_[motors_idx_[ID]]->is_unpacking_) return;    // 若处理回调时则不拷贝防止数据错乱
            memcpy(motors_[motors_idx_[ID]]->rx_buffer_, &data[3], data[3]);
        }
        return;
    }
    // 如果没有注销过电机依旧发生，就是代码逻辑有问题，send bug report tks!
    Crash();
}

/**
 * @brief   对单个电机执行读指令
 * @details 将读取指令发送给电机，范围为调用前的范围
 * @note    函数会清空先前设置的读取范围，若想重新读取必须重新设置
 * @note    函数会将读取范围写入ack_l/h_中供解析时使用
 * @note    函数会设置电机为in_use状态，此时只能设置读取范围但不能对电机执行任何读指令
 */
void MotorSts::TransmitReadCommand()
{
    if (cmd_h_ < cmd_l_) return;
    if (in_use_) return;

    uint8_t idx = 0;
    uart_sts_tx_buffer[idx++] = 0xFF;                       // TxHeader1
    uart_sts_tx_buffer[idx++] = 0xFF;                       // TxHeader2
    uart_sts_tx_buffer[idx++] = ID_;                        // ID
    uart_sts_tx_buffer[idx++] = Special::DUMMY;             // Reserved for FrameLen
    uart_sts_tx_buffer[idx++] = Command::READ;              // Command
    uart_sts_tx_buffer[idx++] = cmd_l_;                     // RegStart
    uart_sts_tx_buffer[idx++] = cmd_h_ - cmd_l_ + 1;        // RegLength
    uart_sts_tx_buffer[3] = idx - 3;                        // FrameLength

    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < idx; i++)
        check_sum += uart_sts_tx_buffer[i];
    uart_sts_tx_buffer[idx++] = ~check_sum;                 // CheckSum

    if (HAL_UART_Transmit_DMA(&huart_sts, uart_sts_tx_buffer, idx) == HAL_OK)
    {
        in_use_ = true;
        ack_l_ = cmd_l_;
        ack_h_ = cmd_h_;
        cmd_h_ = 0x00;
        cmd_l_ = 0xFF;
    }
}

/**
 * @brief   对电机执行一次性写指令
 * @param   r: 需要写入的寄存器地址
 * @param   v: 写入的值
 * @warning 只有需要写入的寄存器是某个16位值的低8位时才能传入>255的数据，否则崩溃
 *          - 如果你真需要利用低位截断的话，在传入时static_cast<uint8_t>(val)
 */
void MotorSts::TransmitWriteCommand(const REG r, uint16_t v) const
{
    const auto is_low_byte_reg = IsLowByteReg(r);
    if (!is_low_byte_reg && (v & 0xFF00) != 0) Crash();     // 传参错误直接跟你爆了(
    v = ConvertStsData(v);

    uint8_t idx = 0;
    uart_sts_tx_buffer[idx++] = 0xFF;                       // TxHeader1
    uart_sts_tx_buffer[idx++] = 0xFF;                       // TxHeader2
    uart_sts_tx_buffer[idx++] = ID_;                        // ID
    uart_sts_tx_buffer[idx++] = Special::DUMMY;             // Reserved for FrameLen
    uart_sts_tx_buffer[idx++] = Command::WRITE;             // Command
    uart_sts_tx_buffer[idx++] = static_cast<uint8_t>(r);    // RegStart
    uart_sts_tx_buffer[idx++] = v;                          // SetValue
    if (is_low_byte_reg)
        uart_sts_tx_buffer[idx++] = v >> 8;
    uart_sts_tx_buffer[3] = idx - 3;                        // FrameLen

    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < idx; i++)
        check_sum += uart_sts_tx_buffer[i];
    uart_sts_tx_buffer[idx++] = ~check_sum;

    HAL_UART_Transmit_DMA(&huart_sts, uart_sts_tx_buffer, idx);
}

/**
 * @brief   批量管理电机，统一写入位置与速度值
 * @details 遍历已注册的电机，如果电机参数已被设置(速度/位置)，就编辑并发送指令，随后重置标志位
 * @note    目前未实现PWM电机端开环控制功能，故将那直接置0问题应该也不大
 */
void MotorSts::ControlAll()
{
    uint8_t idx = 0;
    uart_sts_tx_buffer[idx++] = 0xFF;                       // TxHeader1
    uart_sts_tx_buffer[idx++] = 0xFF;                       // TxHeader2
    uart_sts_tx_buffer[idx++] = Special::MASTER_ID;         // ID
    uart_sts_tx_buffer[idx++] = Special::DUMMY;             // Reserved for FrameLen
    uart_sts_tx_buffer[idx++] = Command::SYN_WRITE;         // Command
    uart_sts_tx_buffer[idx++] = static_cast<uint8_t>(REG::TARGET_POSITION_L); // RegStart
    uart_sts_tx_buffer[idx++] = 0x06;                       // DataLen

    for (uint8_t i = 0; i < motors_count_; i++)
    {
        if (!motors_[i]->is_param_set_) continue;

        uart_sts_tx_buffer[idx++] = motors_[i]->ID_;

        const uint16_t val1 = ConvertStsData(motors_[i]->target_pos_ecd_);
        uart_sts_tx_buffer[idx++] = val1;
        uart_sts_tx_buffer[idx++] = val1 >> 8;

        uart_sts_tx_buffer[idx++] = 0;
        uart_sts_tx_buffer[idx++] = 0;

        const uint16_t val2 = ConvertStsData(motors_[i]->target_vel_ecd_);
        uart_sts_tx_buffer[idx++] = val2;
        uart_sts_tx_buffer[idx++] = val2 >> 8;
    }
    uart_sts_tx_buffer[3] = idx - 3;                        // FrameLen

    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < idx; i++)
        check_sum += uart_sts_tx_buffer[i];
    uart_sts_tx_buffer[idx++] = ~check_sum;

    if (HAL_UART_Transmit_DMA(&huart_sts, uart_sts_tx_buffer, idx) == HAL_OK)
        for (uint8_t i = 0; i < motors_count_; i++)
        {
            // if (!motors_[i]->is_param_set_) continue; // 实际不需要，因为无论过程如何结果都一样()
            motors_[i]->is_param_set_ = false;
        }
}

/**
 * @brief   读取所有电机的参数
 * @param   s: 起始寄存器地址
 * @param   e: 结束寄存器地址(包含该值)
 * @note    该操作同样会覆盖先前设置的寄存器上下界，请留意
 * @warning 程序不会判断读取操作是否安全(比如只读了一个16bit数据的低8位或读了最大角度限制但没读最小角度限制)，需要用户自己确保
 */
void MotorSts::ReadAll(REG s, REG e)
{
    const auto l = static_cast<uint8_t>(s);
    const auto h = static_cast<uint8_t>(e);
    if (l > h) return;
    uint8_t idx = 0;
    uart_sts_tx_buffer[idx++] = 0xFF;                       // TxHeader1
    uart_sts_tx_buffer[idx++] = 0xFF;                       // TxHeader2
    uart_sts_tx_buffer[idx++] = Special::MASTER_ID;         // ID
    uart_sts_tx_buffer[idx++] = Special::DUMMY;             // Reserved for FrameLen
    uart_sts_tx_buffer[idx++] = Command::SYN_READ;          // Command
    uart_sts_tx_buffer[idx++] = l;                          // RegStart
    uart_sts_tx_buffer[idx++] = h - l + 1;                  // DataLen
    for (uint8_t i = 0; i < motors_count_; i++)
    {
        if (motors_[i]->in_use_) continue;
        motors_[i]->SetReadRange(s, e);
        uart_sts_tx_buffer[idx++] = motors_[i]->ID_;
    }
    uart_sts_tx_buffer[3] = idx - 3;                        // FrameLen

    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < idx; i++)
        check_sum += uart_sts_tx_buffer[i];
    uart_sts_tx_buffer[idx++] = ~check_sum;

    if (HAL_UART_Transmit_DMA(&huart_sts, uart_sts_tx_buffer, idx) == HAL_OK)
        for (uint8_t i = 0; i < motors_count_; i++)
        {
            // if (motors_[i]->in_use_) continue; // 实际不需要，因为无论过程如何结果都一样()
            motors_[i]->in_use_ = true;
            motors_[i]->ack_l_ = motors_[i]->cmd_l_;
            motors_[i]->ack_h_ = motors_[i]->cmd_h_;
            motors_[i]->cmd_h_ = 0x00;
            motors_[i]->cmd_l_ = 0xFF;
        }
}

/**
 * @brief   对电机执行批量写指令
 * @param   r: 需要写入的寄存器地址
 * @param   v: 写入的值
 * @note    暂时无法屏蔽某个电机
 * @warning 只有需要写入的寄存器是某个16位值的低8位时才能传入>255的数据，否则崩溃
 *          - 如果你真需要利用低位截断的话，在传入时static_cast<uint8_t>(val)
 */
void MotorSts::WriteAll(const REG r, uint16_t v)
{
    const auto is_low_byte_reg = IsLowByteReg(r);
    if (!is_low_byte_reg && (v & 0xFF00) != 0) Crash();     // 传参错误直接跟你爆了(
    v = ConvertStsData(v);

    uint8_t idx = 0;
    uart_sts_tx_buffer[idx++] = 0xFF;                       // TxHeader1
    uart_sts_tx_buffer[idx++] = 0xFF;                       // TxHeader2
    uart_sts_tx_buffer[idx++] = Special::MASTER_ID;         // ID
    uart_sts_tx_buffer[idx++] = Special::DUMMY;             // Reserved for FrameLen
    uart_sts_tx_buffer[idx++] = Command::SYN_WRITE;         // Command
    uart_sts_tx_buffer[idx++] = static_cast<uint8_t>(r);    // RegStart
    uart_sts_tx_buffer[idx++] = 1 + is_low_byte_reg;        // DataLen

    for (uint8_t i = 0; i < motors_count_; i++)
    {
        uart_sts_tx_buffer[idx++] = motors_[i]->ID_;
        uart_sts_tx_buffer[idx++] = v;
        if (is_low_byte_reg)
            uart_sts_tx_buffer[idx++] = v >> 8;
    }
    uart_sts_tx_buffer[3] = idx - 3;                        // FrameLen

    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < idx; i++)
        check_sum += uart_sts_tx_buffer[i];
    uart_sts_tx_buffer[idx++] = ~check_sum;                 // CheckSum

    HAL_UART_Transmit_DMA(&huart_sts, uart_sts_tx_buffer, idx);
}

/**
 * @brief   初始化电机
 * @details 目前只是读取了所有电机角度的上下界(并会在中断回调中写入)
 * @note    虽然速控电机不需要上下界参数，但实际读取了也并没有影响(反正是没用的)
 */
void MotorSts::Init()
{
    ReadAll(REG::MIN_ANGLE_LIMIT_L,REG::MAX_ANGLE_LIMIT_H);
}
