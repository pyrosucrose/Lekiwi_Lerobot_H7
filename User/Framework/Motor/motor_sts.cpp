//
// Created by Glucose_carbide on 25-8-8.
//

// =============================== 引入头文件 ===============================
#include "motor_sts.hpp"

#include <cstring>

#include "crash.hpp"
#include "delay.hpp"
#include "usartio.hpp"

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
    min_pos_ecd_(min_angle), soft_min_pos_ecd_(static_cast<int16_t>(min_angle - zero_point)),
    max_pos_ecd_(max_angle), soft_max_pos_ecd_(static_cast<int16_t>(max_angle - zero_point))
{
    if (motors_count_ < MAX_MOTORS_COUNT)
    {
        motors_[motors_count_] = this;
        if (motors_idx_[ID] != 0x00 && motors_idx_[ID] != Special::ILLEGAL_ID)  // 没辙，否则idx_必须手动写255个0xFF进去
            Crash();
        motors_idx_[ID] = motors_count_;
        motors_count_++;
    }
    else
    {
        Crash();
    }
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
    Crash();
}

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
    if (data[0] != 0xFF || data[1] != 0xFF)
    {
        return;   // 数据错乱不处理
    }

    const uint8_t param_len = data[3] - 2;
    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < param_len + 5; i++)
        check_sum += data[i];
    check_sum = ~check_sum;
    if (data[param_len + 5] != check_sum)
    {
        return;     // 校验和不匹配不处理
    }

    // const uint8_t ID = data[2];
    // for (uint8_t i = 0; i < motors_count_; i++)
    // {
    //     if (motors_[i]->ID_ == ID)
    //     {
    //         if (data[3] + 2 >= MAX_BUF_LEN)
    //             Crash();
    //         memcpy(motors_[i]->rx_buffer_, &data[3], data[3] + 2);
    //         motors_[i]->received_pack_ = true;
    //         return;
    //     }
    // }

    if (const uint8_t ID = data[2]; motors_[motors_idx_[ID]])
    {
        if (data[3] + 2 >= MAX_BUF_LEN)
            Crash();
        memcpy(motors_[motors_idx_[ID]]->rx_buffer_, &data[3], data[3] + 2);
        motors_[motors_idx_[ID]]->received_pack_ = true;
        return;
    }
    Crash();
}

/**
 * @brief   解析所有电机的数据
 * @details 对于有新数据返回的电机，调用UnpackData来解析
 * @note    该函数应该在主循环或任务中由用户手动调用
 * @todo    目前似乎没有对解析时新数据传入的保护
 */
void MotorSts::UnpackAll()
{
    for (uint8_t i = 0; i < motors_count_; i++)
        if (motors_[i]->received_pack_)
            motors_[i]->UnpackData();
}

/**
 * @brief   解析电机数据
 * @details 解包返回帧，并设置标志位
 * @note    用户可选择开启DEBUG_NODE来看其它数据
 */
void MotorSts::UnpackData()
{
    status_ = rx_buffer_[1]; error_ = status_;  // 错误码处理

    const uint8_t param_len = rx_buffer_[0] - 2;
    for (uint8_t i = 2, cnt = 0; cnt < param_len; i++, cnt++)
    {
        switch(static_cast<REG>(read_reg_l_ + cnt))
        {
        case REG::MIN_ANGLE_LIMIT_L:
            min_pos_ecd_ = PackStsData(rx_buffer_[i], rx_buffer_[i + 1]);
            soft_min_pos_ecd_ = static_cast<int16_t>(min_pos_ecd_ - zero_point_ecd_);
            break;
        case REG::MAX_ANGLE_LIMIT_L:
            max_pos_ecd_ = PackStsData(rx_buffer_[i], rx_buffer_[i + 1]);
            soft_max_pos_ecd_ = static_cast<int16_t>(max_pos_ecd_ - zero_point_ecd_);
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

        if constexpr (DEBUG_MODE)
        {
            switch(static_cast<REG>(read_reg_l_ + cnt))
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
    read_reg_l_ = 0xFF; read_reg_h_ = 0x00;
    received_pack_ = false;
    callback_ready_ = true;
}


void MotorSts::AddReadReg(const REG reg)
{
    const auto addr = static_cast<uint8_t>(reg);
    if (addr < read_reg_l_) read_reg_l_ = addr;
    if (addr > read_reg_h_) read_reg_h_ = addr;
}

void MotorSts::AddReadRangeByCount(const REG start, const uint8_t count)
{
    if (count == 0) return;

    const auto start_addr = static_cast<uint8_t>(start);
    const uint8_t end_addr = start_addr + count - 1;
    if (start_addr < read_reg_l_) read_reg_l_ = start_addr;
    if (end_addr   > read_reg_h_) read_reg_h_ = end_addr;
}

void MotorSts::AddReadRange(const REG start, const REG end)
{
    const auto start_addr = static_cast<uint8_t>(start);
    const auto end_addr = static_cast<uint8_t>(end);
    if (start_addr > end_addr) return;
    if (start_addr < read_reg_l_) read_reg_l_ = start_addr;
    if (end_addr   > read_reg_h_) read_reg_h_ = end_addr;
}

void MotorSts::SetReadRange(REG start, REG end)
{
    read_reg_l_ =  static_cast<uint8_t>(start);
    read_reg_h_ =  static_cast<uint8_t>(end);
}

void MotorSts::TransmitReadCommand() const
{
    if (read_reg_h_ < read_reg_l_) return;

    uint8_t idx = 0;
    uart10_tx_buffer[idx++] = 0xFF;                         // TxHeader1
    uart10_tx_buffer[idx++] = 0xFF;                         // TxHeader2
    uart10_tx_buffer[idx++] = ID_;                          // ID
    uart10_tx_buffer[idx++] = Special::DUMMY;               // Reserved
    uart10_tx_buffer[idx++] = Command::READ;                // Command
    uart10_tx_buffer[idx++] = read_reg_l_;                  // RegStart
    uart10_tx_buffer[idx++] = read_reg_h_ - read_reg_l_ + 1;// RegLength
    uart10_tx_buffer[3] = idx - 3;                          // FrameLength

    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < idx; i++)
        check_sum += uart10_tx_buffer[i];
    uart10_tx_buffer[idx++] = ~check_sum;                   // CheckSum

    HAL_UART_Transmit_DMA(&huart10, uart10_tx_buffer, idx);
}

void MotorSts::TransmitWriteCommand(const REG reg, uint16_t value) const
{
    const auto is_16_bit_reg = IsLowByteRegister(reg);
    if (!is_16_bit_reg && (value & 0xFF00) != 0) Crash();    // 传参错误直接报错
    value = ConvertStsData(value);

    uint8_t idx = 0;
    uart10_tx_buffer[idx++] = 0xFF;                         // TxHeader1
    uart10_tx_buffer[idx++] = 0xFF;                         // TxHeader2
    uart10_tx_buffer[idx++] = ID_;                          // ID
    uart10_tx_buffer[idx++] = Special::DUMMY;               // Reserved
    uart10_tx_buffer[idx++] = Command::WRITE;               // Command
    uart10_tx_buffer[idx++] = static_cast<uint8_t>(reg);    // RegStart
    uart10_tx_buffer[idx++] = value;                        // SetValue
    if (is_16_bit_reg)
        uart10_tx_buffer[idx++] = value >> 8;
    uart10_tx_buffer[3] = idx - 3;                          // FrameLength

    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < idx; i++)
        check_sum += uart10_tx_buffer[i];
    uart10_tx_buffer[idx++] = ~check_sum;

    HAL_UART_Transmit_DMA(&huart10, uart10_tx_buffer, idx);
}

void MotorSts::ControlAll()
{
    uint8_t idx = 0;
    uart10_tx_buffer[idx++] = 0xFF;
    uart10_tx_buffer[idx++] = 0xFF;
    uart10_tx_buffer[idx++] = Special::MASTER_ID;
    uart10_tx_buffer[idx++] = Special::DUMMY; // Reserved
    uart10_tx_buffer[idx++] = Command::SYN_WRITE;
    uart10_tx_buffer[idx++] = static_cast<uint8_t>(REG::TARGET_POSITION_L);
    uart10_tx_buffer[idx++] = 0x06;

    for (uint8_t i = 0; i < motors_count_; i++)
    {
        if (!motors_[i]->is_param_set_) continue;
        const uint16_t val1 = ConvertStsData(motors_[i]->target_pos_ecd_);

        uart10_tx_buffer[idx++] = motors_[i]->ID_;
        uart10_tx_buffer[idx++] = val1;
        uart10_tx_buffer[idx++] = val1 >> 8;

        uart10_tx_buffer[idx++] = 0;
        uart10_tx_buffer[idx++] = 0;

        const uint16_t val2 = ConvertStsData(motors_[i]->target_vel_ecd_);
        uart10_tx_buffer[idx++] = val2;
        uart10_tx_buffer[idx++] = val2 >> 8;
        motors_[i]->is_param_set_ = false;
    }
    uart10_tx_buffer[3] = idx - 3;                          // FrameLength

    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < idx; i++)
        check_sum += uart10_tx_buffer[i];
    uart10_tx_buffer[idx++] = ~check_sum;

    HAL_UART_Transmit_DMA(&huart10, uart10_tx_buffer, idx);
}

void MotorSts::ReadAll(REG start, REG end)
{
    const auto start_addr = static_cast<uint8_t>(start);
    const auto end_addr = static_cast<uint8_t>(end);
    if (start_addr > end_addr) return;
    uint8_t idx = 0;
    uart10_tx_buffer[idx++] = 0xFF;
    uart10_tx_buffer[idx++] = 0xFF;
    uart10_tx_buffer[idx++] = Special::MASTER_ID;
    uart10_tx_buffer[idx++] = Special::DUMMY; // Reserved
    uart10_tx_buffer[idx++] = Command::SYN_READ;
    uart10_tx_buffer[idx++] = start_addr;
    uart10_tx_buffer[idx++] = end_addr - start_addr + 1;
    for (uint8_t i = 0; i < motors_count_; i++)
    {
        if (!motors_[i]->callback_ready_) continue;
        motors_[i]->SetReadRange(start, end);
        uart10_tx_buffer[idx++] = motors_[i]->ID_;
        motors_[i]->callback_ready_ = false;
    }
    uart10_tx_buffer[3] = idx - 3;                          // FrameLength

    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < idx; i++)
        check_sum += uart10_tx_buffer[i];
    uart10_tx_buffer[idx++] = ~check_sum;

    HAL_UART_Transmit_DMA(&huart10, uart10_tx_buffer, idx);
}

void MotorSts::Init()
{
    ReadAll(REG::MIN_ANGLE_LIMIT_L,REG::MAX_ANGLE_LIMIT_H);
}
