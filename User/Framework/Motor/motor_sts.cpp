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
int16_t cMotorSts::PackStsData(const uint8_t L, const uint8_t H)
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
uint16_t cMotorSts::ConvertStsData(const uint16_t s)
{
    return s & 0x8000 ? static_cast<uint16_t>(-(s & 0x7FFF)) : s;
}

/**
 * @brief   表示传入的寄存器地址是否是某个16bit数据的低位寄存器
 * @param   reg:寄存器地址
 * @retval  是不是
 */
bool cMotorSts::Is16BitWriteReg(const REG reg)
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

cMotorSts::cMotorSts(const uint8_t ID, const uint16_t zero_point, const uint16_t min_angle, const uint16_t max_angle, const bool reversed) :
    zero_point_ecd_(zero_point), min_angle_ecd_(min_angle), max_angle_ecd_(max_angle), is_reversed_(reversed), ID_(ID)
{
    if (motors_count_ < MAX_MOTORS_COUNT)
    {
        motors_[motors_count_++] = this;
    }
    else
    {
        Crash();
    }
}

cMotorSts::~cMotorSts()
{
    for (uint8_t i = 0; i < motors_count_; i++)
    {
        if (motors_[i] == this)
        {
            motors_[i] = motors_[motors_count_ - 1]; // 用最后一个覆盖
            motors_[motors_count_ - 1] = nullptr;
            motors_count_--;
            return;
        }
    }
    Crash();
}

void cMotorSts::RxCallback(const uint8_t* data)
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

    const uint8_t ID = data[2];
    for (uint8_t i = 0; i < motors_count_; i++)
    {
        if (motors_[i]->ID_ == ID)
        {
            if (data[3] + 2 < MAX_BUF_LEN)
                memcpy(motors_[i]->rx_buffer_, &data[3], data[3] + 2);
            else
                Crash();
            return;
        }
    }
}

void cMotorSts::UnpackAll()
{
    for (uint8_t i = 0; i < motors_count_; i++)
        motors_[i]->UnpackData();
}

void cMotorSts::UnpackData()
{
    status_ = rx_buffer_[1]; error_ = status_;  // 错误码处理

    const uint8_t param_len = rx_buffer_[0] - 2;
    for (uint8_t i = 2, cnt = 0; cnt < param_len; i++, cnt++)
    {
        switch(static_cast<REG>(read_reg_l_ + cnt))
        {
        case REG::NOW_POS_L:     pos_ecd  = PackStsData(rx_buffer_[i], rx_buffer_[i + 1]); break;
        case REG::NOW_SPEED_L:   vel_ecd_  = PackStsData(rx_buffer_[i], rx_buffer_[i + 1]); break;
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
            case REG::FIRMWARE_MAJOR_VERSION:   usart_printf("[0x%02X] firmware major version: 0x%02X\n", ID_, rx_buffer_[i]);                                         break;
            case REG::FIRMWARE_MINOR_VERSION:   usart_printf("[0x%02X] firmware minor version: 0x%02X\n", ID_, rx_buffer_[i]);                                         break;
            case REG::END_MARKER:               usart_printf("[0x%02X] end marker: 0x%02X\n", ID_, rx_buffer_[i]);                                                     break;
            case REG::SERVO_MAJOR_VERSION:      usart_printf("[0x%02X] servo major version: 0x%02X\n", ID_, rx_buffer_[i]);                                            break;
            case REG::SERVO_MINOR_VERSION:      usart_printf("[0x%02X] servo minor version: 0x%02X\n", ID_, rx_buffer_[i]);                                            break;
            // RW(EPROM)
            case REG::ID:                       usart_printf("[0x%02X] id: 0x%02X\n", ID_, rx_buffer_[i]);                                                             break;
            case REG::BAUD_RATE:                usart_printf("[0x%02X] baud rate: 0x%02X\n", ID_, rx_buffer_[i]);                                                      break;
            case REG::RETURN_DELAY:             usart_printf("[0x%02X] return delay: 0x%02X\n", ID_, rx_buffer_[i]);                                                   break;
            case REG::RESPONSE_STATUS_LEVEL:    usart_printf("[0x%02X] response status level: 0x%02X\n", ID_, rx_buffer_[i]);                                          break;
            case REG::MIN_ANGLE_LIMIT_L:        usart_printf("[0x%02X] min angle limit: 0x%04X\n", ID_, static_cast<uint16_t>(rx_buffer_[i + 1] << 8 | rx_buffer_[i]));      break;
            case REG::MAX_ANGLE_LIMIT_L:        usart_printf("[0x%02X] max angle limit: 0x%04X\n", ID_, static_cast<uint16_t>(rx_buffer_[i + 1] << 8 | rx_buffer_[i]));      break;
            case REG::MAX_TEMPERATURE:          usart_printf("[0x%02X] max temperature: 0x%02X\n", ID_, rx_buffer_[i]);                                                break;
            case REG::MAX_VOLTAGE:              usart_printf("[0x%02X] max voltage: 0x%02X\n", ID_, rx_buffer_[i]);                                                    break;
            case REG::MIN_VOLTAGE:              usart_printf("[0x%02X] min voltage: 0x%02X\n", ID_, rx_buffer_[i]);                                                    break;
            case REG::MAX_TORQUE_L:             usart_printf("[0x%02X] max torque: 0x%04X\n", ID_, static_cast<uint16_t>(rx_buffer_[i + 1] << 8 | rx_buffer_[i]));           break;
            case REG::PHASE:                    usart_printf("[0x%02X] phase: 0x%02X\n", ID_, rx_buffer_[i]);                                                          break;
            case REG::UNLOAD_CONDITION:         usart_printf("[0x%02X] unload condition: 0x%02X\n", ID_, rx_buffer_[i]);                                               break;
            case REG::LED_ALARM_CONDITION:      usart_printf("[0x%02X] led alarm condition: 0x%02X\n", ID_, rx_buffer_[i]);                                            break;
            case REG::POS_P_GAIN:               usart_printf("[0x%02X] pos p gain: 0x%02X\n", ID_, rx_buffer_[i]);                                                     break;
            case REG::POS_D_GAIN:               usart_printf("[0x%02X] pos d gain: 0x%02X\n", ID_, rx_buffer_[i]);                                                     break;
            case REG::POS_I_GAIN:               usart_printf("[0x%02X] pos i gain: 0x%02X\n", ID_, rx_buffer_[i]);                                                     break;
            case REG::MIN_START_TORQUE:         usart_printf("[0x%02X] min start torque: 0x%02X\n", ID_, rx_buffer_[i]);                                               break;
            case REG::INTEGRAL_LIMIT:           usart_printf("[0x%02X] integral limit: 0x%02X\n", ID_, rx_buffer_[i]);                                                 break;
            case REG::CW_DEAD_ZONE:             usart_printf("[0x%02X] cw dead zone: 0x%02X\n", ID_, rx_buffer_[i]);                                                   break;
            case REG::CCW_DEAD_ZONE:            usart_printf("[0x%02X] ccw dead zone: 0x%02X\n", ID_, rx_buffer_[i]);                                                  break;
            case REG::PROTECTION_CURRENT_L:     usart_printf("[0x%02X] protection current: 0x%02X\n", ID_, static_cast<uint16_t>(rx_buffer_[i + 1] << 8 | rx_buffer_[i]));   break;
            case REG::ANGLE_RESOLUTION:         usart_printf("[0x%02X] angle resolution: 0x%02X\n", ID_, rx_buffer_[i]);                                               break;
            case REG::POSITION_CORRECTION_L:    usart_printf("[0x%02X] position correction: 0x%04X\n", ID_, static_cast<int16_t>(rx_buffer_[i + 1] << 8 | rx_buffer_[i]));   break;
            case REG::OPERATION_MODE:           usart_printf("[0x%02X] operation mode: 0x%02X\n", ID_, rx_buffer_[i]);                                                 break;
            case REG::PROTECTION_TORQUE:        usart_printf("[0x%02X] protection torque: 0x%02X\n", ID_, rx_buffer_[i]);                                              break;
            case REG::PROTECTION_TIME:          usart_printf("[0x%02X] protection time: 0x%02X\n", ID_, rx_buffer_[i]);                                                break;
            case REG::OVERLOAD_TORQUE:          usart_printf("[0x%02X] overload torque: 0x%02X\n", ID_, rx_buffer_[i]);                                                break;
            case REG::SPEED_P_GAIN:             usart_printf("[0x%02X] speed p gain: 0x%02X\n", ID_, rx_buffer_[i]);                                                   break;
            case REG::OVERCURRENT_PROTECT_TIME: usart_printf("[0x%02X] overcurrent protect time: 0x%02X\n", ID_, rx_buffer_[i]);                                       break;
            case REG::SPEED_I_GAIN:             usart_printf("[0x%02X] speed i gain: 0x%02X\n", ID_, rx_buffer_[i]);                                                   break;
            // RW(SRAM)
            case REG::TORQUE_SWITCH:            usart_printf("[0x%02X] torque switch: 0x%02X\n", ID_, rx_buffer_[i]);                                                  break;
            case REG::ACCELERATION:             usart_printf("[0x%02X] acceleration: 0x%02X\n", ID_, rx_buffer_[i]);                                                   break;
            case REG::TARGET_POSITION_L:        usart_printf("[0x%02X] goal position: 0x%04X\n", ID_, static_cast<int16_t>(rx_buffer_[i + 1] << 8 | rx_buffer_[i]));         break;
            case REG::MOVING_TIME_L:            usart_printf("[0x%02X] moving time: 0x%04X\n", ID_, static_cast<uint16_t>(rx_buffer_[i + 1] << 8 | rx_buffer_[i]));          break;
            case REG::TARGET_SPEED_L:           usart_printf("[0x%02X] goal speed: 0x%04X\n", ID_, static_cast<int16_t>(rx_buffer_[i + 1] << 8 | rx_buffer_[i]));            break;
            case REG::TORQUE_LIMIT_L:           usart_printf("[0x%02X] torque limit: 0x%04X\n", ID_, static_cast<uint16_t>(rx_buffer_[i + 1] << 8 | rx_buffer_[i]));         break;
            case REG::LOCK:                     usart_printf("[0x%02X] lock: 0x%02X\n", ID_, rx_buffer_[i]);                                                           break;
            // RO(SRAM)
            case REG::NOW_POS_L:                usart_printf("[0x%02X] pos: 0x%04X\n", ID_, static_cast<uint16_t>(rx_buffer_[i + 1] << 8 | rx_buffer_[i]));                  break;
            case REG::NOW_SPEED_L:              usart_printf("[0x%02X] speed: 0x%04X\n", ID_, static_cast<uint16_t>(rx_buffer_[i + 1] << 8 | rx_buffer_[i]));                break;
            case REG::NOW_LOAD_L:               usart_printf("[0x%02X] load: 0x%04X\n", ID_, static_cast<uint16_t>(rx_buffer_[i + 1] << 8 | rx_buffer_[i]));                 break;
            case REG::NOW_VOLT:                 usart_printf("[0x%02X] volt: 0x%02X\n", ID_, rx_buffer_[i]);                                                           break;
            case REG::NOW_TEMP:                 usart_printf("[0x%02X] temp: 0x%02X\n", ID_, rx_buffer_[i]);                                                           break;
            case REG::ASYNCHRONOUS_WRITE:       usart_printf("[0x%02X] asynchronous write: 0x%02X\n", ID_, rx_buffer_[i]);                                             break;
            case REG::STATUS:                   usart_printf("[0x%02X] status: 0x%02X\n", ID_, rx_buffer_[i]);                                                         break;
            case REG::IS_MOVING:                usart_printf("[0x%02X] is moving: 0x%02X\n", ID_, rx_buffer_[i]);                                                      break;
            case REG::NOW_CURRENT_L:            usart_printf("[0x%02X] current: 0x%04X\n", ID_, static_cast<uint16_t>(rx_buffer_[i + 1] << 8 | rx_buffer_[i]));              break;
            default: break;
            }
        }
    }
    read_reg_l_ = 0xFF; read_reg_h_ = 0x00;
}


void cMotorSts::AddReadReg(const REG reg)
{
    const auto addr = static_cast<uint8_t>(reg);
    if (addr < read_reg_l_) read_reg_l_ = addr;
    if (addr > read_reg_h_) read_reg_h_ = addr;
}

void cMotorSts::AddReadRangeByCount(const REG start, const uint8_t count)
{
    if (count == 0) return;

    const auto start_addr = static_cast<uint8_t>(start);
    const uint8_t end_addr = start_addr + count - 1;
    if (start_addr < read_reg_l_) read_reg_l_ = start_addr;
    if (end_addr   > read_reg_h_) read_reg_h_ = end_addr;
}

void cMotorSts::AddReadRange(const REG start, const REG end)
{
    const auto start_addr = static_cast<uint8_t>(start);
    const auto end_addr = static_cast<uint8_t>(end);
    if (start_addr > end_addr) return;
    if (start_addr < read_reg_l_) read_reg_l_ = start_addr;
    if (end_addr   > read_reg_h_) read_reg_h_ = end_addr;
}

void cMotorSts::SetReadRange(REG start, REG end)
{
    read_reg_l_ =  static_cast<uint8_t>(start);
    read_reg_h_ =  static_cast<uint8_t>(end);
}

void cMotorSts::TransmitReadCommand() const
{
    if (read_reg_h_ < read_reg_l_) return;

    uint8_t idx = 0;
    uart10_tx_buffer[idx++] = 0xFF;                         // TxHeader1
    uart10_tx_buffer[idx++] = 0xFF;                         // TxHeader2
    uart10_tx_buffer[idx++] = ID_;                           // ID
    uart10_tx_buffer[idx++] = Special::DUMMY;               // Reserved
    uart10_tx_buffer[idx++] = Command::READ;                // Command
    uart10_tx_buffer[idx++] = read_reg_l_;                   // RegStart
    uart10_tx_buffer[idx++] = read_reg_h_ - read_reg_l_ + 1;  // RegLength
    uart10_tx_buffer[3] = idx - 3;                          // FrameLength

    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < idx; i++)
        check_sum += uart10_tx_buffer[i];
    uart10_tx_buffer[idx++] = ~check_sum;                   // CheckSum

    HAL_UART_Transmit_DMA(&huart10, uart10_tx_buffer, idx);
}

void cMotorSts::TransmitWriteCommand(const REG reg, uint16_t value) const
{
    const auto is_16_bit_reg = Is16BitWriteReg(reg);
    if (!is_16_bit_reg && (value & 0xFF00) != 0) return;    // 传参错误不处理
    value = ConvertStsData(value);

    uint8_t idx = 0;
    uart10_tx_buffer[idx++] = 0xFF;                         // TxHeader1
    uart10_tx_buffer[idx++] = 0xFF;                         // TxHeader2
    uart10_tx_buffer[idx++] = ID_;                           // ID
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

void cMotorSts::ControlAll()
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
        const uint16_t val1 = ConvertStsData(motors_[i]->target_pos_);

        uart10_tx_buffer[idx++] = motors_[i]->ID_;
        uart10_tx_buffer[idx++] = val1;
        uart10_tx_buffer[idx++] = val1 >> 8;

        uart10_tx_buffer[idx++] = 0;
        uart10_tx_buffer[idx++] = 0;

        const uint16_t val2 = ConvertStsData(motors_[i]->target_vel_);
        uart10_tx_buffer[idx++] = val2;
        uart10_tx_buffer[idx++] = val2 >> 8;
    }
    uart10_tx_buffer[3] = idx - 3;                          // FrameLength

    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < idx; i++)
        check_sum += uart10_tx_buffer[i];
    uart10_tx_buffer[idx++] = ~check_sum;

    HAL_UART_Transmit_DMA(&huart10, uart10_tx_buffer, idx);
}

void cMotorSts::ReadAll(REG start, REG end)
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
        motors_[i]->SetReadRange(start, end);
        uart10_tx_buffer[idx++] = motors_[i]->ID_;
    }
    uart10_tx_buffer[3] = idx - 3;                          // FrameLength

    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < idx; i++)
        check_sum += uart10_tx_buffer[i];
    uart10_tx_buffer[idx++] = ~check_sum;

    HAL_UART_Transmit_DMA(&huart10, uart10_tx_buffer, idx);
}
