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
 * @brief   批量解包
 * @details 对于有新数据返回的电机，调用UnpackData来解析
 * @note    该函数应该在主循环或任务中由用户手动调用
 */
void MotorSts::UnpackAll()
{
    for (uint8_t i = 0; i < motors_count_; i++)
        if (motors_[i]->received_pack_)
            motors_[i]->UnpackData();
}

/**
 * @brief   解析电机接收数据
 * @details 解包返回帧，并设置标志位
 * @note    用户可选择开启DEBUG_MODE来看其它数据
 */
void MotorSts::UnpackData()
{
    is_unpacking_ = true;
    status_ = rx_buffer_[1]; error_ = status_;  // 错误码处理

    bool boundary_flag = false;
    for (uint8_t i = 2, cnt = 0; cnt < rx_buffer_[0] - 2; i++, cnt++)
    {
        switch(static_cast<REG>(ack_l_ + cnt))
        {
        case REG::MIN_ANGLE_LIMIT_L:
            min_pos_ecd_ = PackStsData(rx_buffer_[i], rx_buffer_[i + 1]);
            boundary_flag = true;
            break;
        case REG::MAX_ANGLE_LIMIT_L:
            max_pos_ecd_ = PackStsData(rx_buffer_[i], rx_buffer_[i + 1]);
            boundary_flag = true;
            break;
        case REG::NOW_POS_L:
            pos_ecd_  = PackStsData(rx_buffer_[i], rx_buffer_[i + 1]);
            soft_pos_ecd = static_cast<int16_t>(is_reversed_ ? zero_point_ecd_ - pos_ecd_ : zero_point_ecd_ + pos_ecd_);
            break;
        case REG::NOW_SPEED_L:
            vel_ecd_  = PackStsData(rx_buffer_[i], rx_buffer_[i + 1]);
            soft_vel_ecd = static_cast<int16_t>(is_reversed_ ? -vel_ecd_ : vel_ecd_);
            break;
        case REG::NOW_LOAD_L:    load_ecd_ = PackStsData(rx_buffer_[i], rx_buffer_[i + 1]); break;
        case REG::NOW_VOLT:      volt_ecd_ = rx_buffer_[i]; break;
        case REG::NOW_TEMP:      temp_ecd_ = rx_buffer_[i]; break;
        case REG::NOW_CURRENT_L: cur_ecd_  = PackStsData(rx_buffer_[i], rx_buffer_[i + 1]); break;
        default: break;
        }
        if (boundary_flag)
        {
            soft_min_pos_ecd_ = static_cast<int16_t>(is_reversed_ ? zero_point_ecd_ - max_pos_ecd_ : min_pos_ecd_ - zero_point_ecd_);
            soft_max_pos_ecd_ = static_cast<int16_t>(is_reversed_ ? zero_point_ecd_ - min_pos_ecd_ : max_pos_ecd_ - zero_point_ecd_);
        }

        if constexpr (DEBUG_MODE)
        {
            switch(static_cast<REG>(ack_l_ + cnt))
            {
            // RO(EPROM)
            case REG::FIRMWARE_MAJOR_VERSION:   usart_printf("[0x%02X] firmware major version: 0x%02X\n", ID_, rx_buffer_[i]);                                     break;
            case REG::FIRMWARE_MINOR_VERSION:   usart_printf("[0x%02X] firmware minor version: 0x%02X\n", ID_, rx_buffer_[i]);                                     break;
            case REG::END_MARKER:               usart_printf("[0x%02X] end marker: 0x%02X\n", ID_, rx_buffer_[i]);                                                 break;
            case REG::SERVO_MAJOR_VERSION:      usart_printf("[0x%02X] servo major version: 0x%02X\n", ID_, rx_buffer_[i]);                                        break;
            case REG::SERVO_MINOR_VERSION:      usart_printf("[0x%02X] servo minor version: 0x%02X\n", ID_, rx_buffer_[i]);                                        break;
            // RW(EPROM)
            case REG::ID:                       usart_printf("[0x%02X] id: 0x%02X\n", ID_, rx_buffer_[i]);                                                         break;
            case REG::BAUD_RATE:                usart_printf("[0x%02X] baud rate: 0x%02X\n", ID_, rx_buffer_[i]);                                                  break;
            case REG::RETURN_DELAY:             usart_printf("[0x%02X] return delay: 0x%02X\n", ID_, rx_buffer_[i]);                                               break;
            case REG::RESPONSE_STATUS_LEVEL:    usart_printf("[0x%02X] response status level: 0x%02X\n", ID_, rx_buffer_[i]);                                      break;
            case REG::MIN_ANGLE_LIMIT_L:        usart_printf("[0x%02X] min angle limit: %d\n", ID_, PackStsData(rx_buffer_[i], rx_buffer_[i + 1]));                break;
            case REG::MAX_ANGLE_LIMIT_L:        usart_printf("[0x%02X] max angle limit: %d\n", ID_, PackStsData(rx_buffer_[i], rx_buffer_[i + 1]));                break;
            case REG::MAX_TEMPERATURE:          usart_printf("[0x%02X] max temperature: 0x%02X\n", ID_, rx_buffer_[i]);                                            break;
            case REG::MAX_VOLTAGE:              usart_printf("[0x%02X] max voltage: 0x%02X\n", ID_, rx_buffer_[i]);                                                break;
            case REG::MIN_VOLTAGE:              usart_printf("[0x%02X] min voltage: 0x%02X\n", ID_, rx_buffer_[i]);                                                break;
            case REG::MAX_TORQUE_L:             usart_printf("[0x%02X] max torque: %d\n", ID_, PackStsData(rx_buffer_[i], rx_buffer_[i + 1]));                     break;
            case REG::PHASE:                    usart_printf("[0x%02X] phase: 0x%02X\n", ID_, rx_buffer_[i]);                                                      break;
            case REG::UNLOAD_CONDITION:         usart_printf("[0x%02X] unload condition: 0x%02X\n", ID_, rx_buffer_[i]);                                           break;
            case REG::LED_ALARM_CONDITION:      usart_printf("[0x%02X] led alarm condition: 0x%02X\n", ID_, rx_buffer_[i]);                                        break;
            case REG::POS_P_GAIN:               usart_printf("[0x%02X] pos p gain: 0x%02X\n", ID_, rx_buffer_[i]);                                                 break;
            case REG::POS_D_GAIN:               usart_printf("[0x%02X] pos d gain: 0x%02X\n", ID_, rx_buffer_[i]);                                                 break;
            case REG::POS_I_GAIN:               usart_printf("[0x%02X] pos i gain: 0x%02X\n", ID_, rx_buffer_[i]);                                                 break;
            case REG::MIN_START_TORQUE:         usart_printf("[0x%02X] min start torque: 0x%02X\n", ID_, rx_buffer_[i]);                                           break;
            case REG::INTEGRAL_LIMIT:           usart_printf("[0x%02X] integral limit: 0x%02X\n", ID_, rx_buffer_[i]);                                             break;
            case REG::CW_DEAD_ZONE:             usart_printf("[0x%02X] cw dead zone: 0x%02X\n", ID_, rx_buffer_[i]);                                               break;
            case REG::CCW_DEAD_ZONE:            usart_printf("[0x%02X] ccw dead zone: 0x%02X\n", ID_, rx_buffer_[i]);                                              break;
            case REG::PROTECTION_CURRENT_L:     usart_printf("[0x%02X] protection current: %d\n", ID_, PackStsData(rx_buffer_[i], rx_buffer_[i + 1]));             break;
            case REG::ANGLE_RESOLUTION:         usart_printf("[0x%02X] angle resolution: 0x%02X\n", ID_, rx_buffer_[i]);                                           break;
            case REG::POSITION_CORRECTION_L:    usart_printf("[0x%02X] position correction: %d\n", ID_, PackStsData(rx_buffer_[i], rx_buffer_[i + 1]));            break;
            case REG::OPERATION_MODE:           usart_printf("[0x%02X] operation mode: 0x%02X\n", ID_, rx_buffer_[i]);                                             break;
            case REG::PROTECTION_TORQUE:        usart_printf("[0x%02X] protection torque: 0x%02X\n", ID_, rx_buffer_[i]);                                          break;
            case REG::PROTECTION_TIME:          usart_printf("[0x%02X] protection time: 0x%02X\n", ID_, rx_buffer_[i]);                                            break;
            case REG::OVERLOAD_TORQUE:          usart_printf("[0x%02X] overload torque: 0x%02X\n", ID_, rx_buffer_[i]);                                            break;
            case REG::SPEED_P_GAIN:             usart_printf("[0x%02X] speed p gain: 0x%02X\n", ID_, rx_buffer_[i]);                                               break;
            case REG::OVERCURRENT_PROTECT_TIME: usart_printf("[0x%02X] overcurrent protect time: 0x%02X\n", ID_, rx_buffer_[i]);                                   break;
            case REG::SPEED_I_GAIN:             usart_printf("[0x%02X] speed i gain: 0x%02X\n", ID_, rx_buffer_[i]);                                               break;
            // RW(SRAM)
            case REG::TORQUE_SWITCH:            usart_printf("[0x%02X] torque switch: 0x%02X\n", ID_, rx_buffer_[i]);                                              break;
            case REG::ACCELERATION:             usart_printf("[0x%02X] acceleration: 0x%02X\n", ID_, rx_buffer_[i]);                                               break;
            case REG::TARGET_POSITION_L:        usart_printf("[0x%02X] goal position: %d\n", ID_, PackStsData(rx_buffer_[i], rx_buffer_[i + 1]));                  break;
            case REG::MOVING_TIME_L:            usart_printf("[0x%02X] moving time: %d\n", ID_, PackStsData(rx_buffer_[i], rx_buffer_[i + 1]));                    break;
            case REG::TARGET_SPEED_L:           usart_printf("[0x%02X] goal speed: %d\n", ID_, PackStsData(rx_buffer_[i], rx_buffer_[i + 1]));                     break;
            case REG::TORQUE_LIMIT_L:           usart_printf("[0x%02X] torque limit: %d\n", ID_, PackStsData(rx_buffer_[i], rx_buffer_[i + 1]));                   break;
            case REG::LOCK:                     usart_printf("[0x%02X] lock: 0x%02X\n", ID_, rx_buffer_[i]);                                                       break;
            // RO(SRAM)
            case REG::NOW_POS_L:                usart_printf("[0x%02X] pos: %d\n", ID_, PackStsData(rx_buffer_[i], rx_buffer_[i + 1]));                            break;
            case REG::NOW_SPEED_L:              usart_printf("[0x%02X] speed: %d\n", ID_, PackStsData(rx_buffer_[i], rx_buffer_[i + 1]));                          break;
            case REG::NOW_LOAD_L:               usart_printf("[0x%02X] load: %d\n", ID_, PackStsData(rx_buffer_[i], rx_buffer_[i + 1]));                           break;
            case REG::NOW_VOLT:                 usart_printf("[0x%02X] volt: 0x%02X\n", ID_, rx_buffer_[i]);                                                       break;
            case REG::NOW_TEMP:                 usart_printf("[0x%02X] temp: 0x%02X\n", ID_, rx_buffer_[i]);                                                       break;
            case REG::ASYNCHRONOUS_WRITE:       usart_printf("[0x%02X] asynchronous write: 0x%02X\n", ID_, rx_buffer_[i]);                                         break;
            case REG::STATUS:                   usart_printf("[0x%02X] status: 0x%02X\n", ID_, rx_buffer_[i]);                                                     break;
            case REG::IS_MOVING:                usart_printf("[0x%02X] is moving: 0x%02X\n", ID_, rx_buffer_[i]);                                                  break;
            case REG::NOW_CURRENT_L:            usart_printf("[0x%02X] current: %d\n", ID_, PackStsData(rx_buffer_[i], rx_buffer_[i + 1]));                        break;
            default: break;
            }
        }
    }
    ack_l_ = 0xFF; ack_h_ = 0x00;
    received_pack_ = false;
    in_use_ = false;
    is_unpacking_ = false;
}

/**
 * @brief   添加需要读取的寄存器
 * @param   r: 寄存器
 * @note    协议要求读取的寄存器地址必须连续，故函数修改的是需要读取的最小/最大寄存器
 *          - 比如你只需要读0x20和0x30的寄存器，但发送时必须要求电机返回0x20-0x30的所有寄存器值
 * @warning 程序不会判断读取操作是否安全(比如只读了一个16bit数据的低8位或读了最大角度限制但没读最小角度限制)，需要用户自己确保
 */
void MotorSts::AddReadReg(const REG r)
{
    const auto reg = static_cast<uint8_t>(r);
    if (reg < cmd_l_) cmd_l_ = reg;
    if (reg > cmd_h_) cmd_h_ = reg;
}

/**
 * @brief   添加需要读取的寄存器
 * @param   s: 从哪个寄存器开始读
 * @param   c: 一共读几个(最少一个)
 * @note    协议要求读取的寄存器地址必须连续，故函数修改的是需要读取的最小/最大寄存器
 *          - 比如你只需要读0x20和0x30的寄存器，但发送时必须要求电机返回0x20-0x30的所有寄存器值
 * @warning 程序不会判断读取操作是否安全(比如只读了一个16bit数据的低8位或读了最大角度限制但没读最小角度限制)，需要用户自己确保
 */
void MotorSts::AddReadRangeByCount(const REG s, const uint8_t c)
{
    if (c == 0) return;

    const auto l = static_cast<uint8_t>(s);
    const uint8_t h = l + c - 1;
    if (l < cmd_l_) cmd_l_ = l;
    if (h > cmd_h_) cmd_h_ = h;
}

/**
 * @brief   添加需要读取的寄存器
 * @param   s: 从哪个寄存器开始读
 * @param   e: 读到哪个寄存器(包含该寄存器)
 * @note    协议要求读取的寄存器地址必须连续，故函数修改的是需要读取的最小/最大寄存器
 *          - 比如你只需要读0x20和0x30的寄存器，但发送时必须要求电机返回0x20-0x30的所有寄存器值
 * @warning 程序不会判断读取操作是否安全(比如只读了一个16bit数据的低8位或读了最大角度限制但没读最小角度限制)，需要用户自己确保
 */
void MotorSts::AddReadRange(const REG s, const REG e)
{
    const auto l = static_cast<uint8_t>(s);
    const auto h = static_cast<uint8_t>(e);
    if (l > h) return;
    if (l < cmd_l_) cmd_l_ = l;
    if (h > cmd_h_) cmd_h_ = h;
}

/**
 * @brief   设置需要读取的寄存器
 * @param   s: 从哪个寄存器开始读
 * @param   e: 读到哪个寄存器(包含该寄存器)
 * @warning 程序不会判断读取操作是否安全(比如只读了一个16bit数据的低8位或读了最大角度限制但没读最小角度限制)，需要用户自己确保
 * @warning 该操作会覆盖上面三个AddRead的操作，若你只是需要添加而不是覆盖，请不要调用这个
 */
void MotorSts::SetReadRange(REG s, REG e)
{
    cmd_l_ = static_cast<uint8_t>(s);
    cmd_h_ = static_cast<uint8_t>(e);
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
        const uint16_t val1 = ConvertStsData(motors_[i]->target_pos_ecd_);

        uart_sts_tx_buffer[idx++] = motors_[i]->ID_;
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
            motors_[i]->is_param_set_ = false;
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
