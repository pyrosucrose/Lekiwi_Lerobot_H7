//
// Created by Glucose_carbide on 25-8-8.
//

// =============================== 引入头文件 ===============================
#include "motor_sts.hpp"
#include "usartio.hpp"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ==================================

// =============================== 函数实现 ===============================

cMotorSts::CALLBACK_TYPE cMotorSts::RxCallback(const uint8_t* data)
{
    if (data[2] != ID) return CALLBACK_TYPE::WRONG_ID;                                  // serial_ID匹配才处理
    if (data[0] != 0xFF || data[1] != 0xFF) return CALLBACK_TYPE::WRONG_HEAD;           // 数据错乱不处理
    const uint8_t param_len = data[3] - 2;
    if (param_len != read_reg_h - read_reg_l + 1) return CALLBACK_TYPE::WRONG_LENGTH;   // 参数长度不匹配不处理
    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < param_len + 5; i++)
        check_sum += data[i];
    check_sum = ~check_sum;
    if (data[param_len + 5] != check_sum) return CALLBACK_TYPE::WRONG_CHECKSUM;         // 校验和不匹配不处理

    status = data[4]; error = status;  // 错误码处理

    for (uint8_t i = 5, cnt = 0; cnt < param_len; i++, cnt++)
    {
        switch(static_cast<REG>(read_reg_l + cnt))
        {
        case REG::FIRMWARE_MAJOR_VERSION:
            usart_printf("[0x%02X] firmware major version: 0x%02X\n", ID, data[i]); break;
        case REG::FIRMWARE_MINOR_VERSION:
            usart_printf("[0x%02X] firmware minor version: 0x%02X\n", ID, data[i]); break;
        case REG::END_MARKER:
            usart_printf("[0x%02X] end marker: 0x%02X\n", ID, data[i]); break;
        case REG::SERVO_MAJOR_VERSION:
            usart_printf("[0x%02X] servo major version: 0x%02X\n", ID, data[i]); break;
        case REG::SERVO_MINOR_VERSION:
            usart_printf("[0x%02X] servo minor version: 0x%02X\n", ID, data[i]); break;

        case REG::ID:
            usart_printf("[0x%02X] id: 0x%02X\n", ID, data[i]); break;
        case REG::BAUD_RATE:
            usart_printf("[0x%02X] baud rate: 0x%02X\n", ID, data[i]); break;
        case REG::RETURN_DELAY:
            usart_printf("[0x%02X] return delay: 0x%02X\n", ID, data[i]); break;
        case REG::RESPONSE_STATUS_LEVEL:
            usart_printf("[0x%02X] response status level: 0x%02X\n", ID, data[i]); break;
        case REG::MIN_ANGLE_LIMIT_L:
            usart_printf("[0x%02X] min angle limit: 0x%04X\n", ID, static_cast<uint16_t>(data[i + 1] << 8 | data[i])); break;
        case REG::MAX_ANGLE_LIMIT_L:
            usart_printf("[0x%02X] max angle limit: 0x%04X\n", ID, static_cast<uint16_t>(data[i + 1] << 8 | data[i])); break;
        case REG::MAX_TEMPERATURE:
            usart_printf("[0x%02X] max temperature: 0x%02X\n", ID, data[i]); break;
        case REG::MAX_VOLTAGE:
            usart_printf("[0x%02X] max voltage: 0x%02X\n", ID, data[i]); break;
        case REG::MIN_VOLTAGE:
            usart_printf("[0x%02X] min voltage: 0x%02X\n", ID, data[i]); break;
        case REG::MAX_TORQUE_L:
            usart_printf("[0x%02X] max torque: 0x%04X\n", ID, static_cast<uint16_t>(data[i + 1] << 8 | data[i])); break;
        case REG::PHASE:
            usart_printf("[0x%02X] phase: 0x%02X\n", ID, data[i]); break;
        case REG::UNLOAD_CONDITION:
            usart_printf("[0x%02X] unload condition: 0x%02X\n", ID, data[i]); break;
        case REG::LED_ALARM_CONDITION:
            usart_printf("[0x%02X] led alarm condition: 0x%02X\n", ID, data[i]); break;
        case REG::POS_P_GAIN:
            usart_printf("[0x%02X] pos p gain: 0x%02X\n", ID, data[i]); break;
        case REG::POS_D_GAIN:
            usart_printf("[0x%02X] pos d gain: 0x%02X\n", ID, data[i]); break;
        case REG::POS_I_GAIN:
            usart_printf("[0x%02X] pos i gain: 0x%02X\n", ID, data[i]); break;
        case REG::MIN_START_TORQUE:
            usart_printf("[0x%02X] min start torque: 0x%02X\n", ID, data[i]); break;
        case REG::INTEGRAL_LIMIT:
            usart_printf("[0x%02X] integral limit: 0x%02X\n", ID, data[i]); break;
        case REG::CW_DEAD_ZONE:
            usart_printf("[0x%02X] cw dead zone: 0x%02X\n", ID, data[i]); break;
        case REG::CCW_DEAD_ZONE:
            usart_printf("[0x%02X] ccw dead zone: 0x%02X\n", ID, data[i]); break;
        case REG::PROTECTION_CURRENT_L:
            usart_printf("[0x%02X] protection current: 0x%02X\n", ID,
                static_cast<uint16_t>(data[i + 1] << 8 | data[i])); break;
        case REG::ANGLE_RESOLUTION:
            usart_printf("[0x%02X] angle resolution: 0x%02X\n", ID, data[i]); break;
        case REG::POSITION_CORRECTION_L:
            usart_printf("[0x%02X] position correction: 0x%04X\n", ID, static_cast<int16_t>(data[i + 1] << 8 | data[i])); break;
        case REG::OPERATION_MODE:
            usart_printf("[0x%02X] operation mode: 0x%02X\n", ID, data[i]); break;
        case REG::PROTECTION_TORQUE:
            usart_printf("[0x%02X] protection torque: 0x%02X\n", ID, data[i]); break;
        case REG::PROTECTION_TIME:
            usart_printf("[0x%02X] protection time: 0x%02X\n", ID, data[i]); break;
        case REG::OVERLOAD_TORQUE:
            usart_printf("[0x%02X] overload torque: 0x%02X\n", ID, data[i]); break;
        case REG::SPEED_P_GAIN:
            usart_printf("[0x%02X] speed p gain: 0x%02X\n", ID, data[i]); break;
        case REG::OVERCURRENT_PROTECT_TIME:
            usart_printf("[0x%02X] overcurrent protect time: 0x%02X\n", ID, data[i]); break;
        case REG::SPEED_I_GAIN:
            usart_printf("[0x%02X] speed i gain: 0x%02X\n", ID, data[i]); break;

        case REG::TORQUE_SWITCH:
            usart_printf("[0x%02X] torque switch: 0x%02X\n", ID, data[i]); break;
        case REG::ACCELERATION:
            usart_printf("[0x%02X] acceleration: 0x%02X\n", ID, data[i]); break;
        case REG::TARGET_POSITION_L:
            usart_printf("[0x%02X] goal position: 0x%04X\n", ID, static_cast<int16_t>(data[i + 1] << 8 | data[i])); break;
        case REG::MOVING_TIME_L:
            usart_printf("[0x%02X] moving time: 0x%04X\n", ID, static_cast<uint16_t>(data[i + 1] << 8 | data[i])); break;
        case REG::TARGET_SPEED_L:
            usart_printf("[0x%02X] goal speed: 0x%04X\n", ID, static_cast<int16_t>(data[i + 1] << 8 | data[i])); break;
        case REG::TORQUE_LIMIT_L:
            usart_printf("[0x%02X] torque limit: 0x%04X\n", ID, static_cast<uint16_t>(data[i + 1] << 8 | data[i])); break;
        case REG::LOCK:
            usart_printf("[0x%02X] lock: 0x%02X\n", ID, data[i]); break;

        case REG::NOW_POS_L:
            pos_ecd = static_cast<int16_t>(data[i + 1] << 8 | data[i]); break;
        case REG::NOW_SPEED_L:
            vel_ecd = static_cast<int16_t>(data[i + 1] << 8 | data[i]);
            if ((vel_ecd & 0x8000) != 0) vel_ecd = static_cast<int16_t>(-(vel_ecd & 0x7FFF));
            break;
        case REG::NOW_LOAD_L:
            load_ecd = data[i + 1] << 8 | data[i]; break;
        case REG::NOW_VOLT:
            volt_ecd = data[i]; break;
        case REG::NOW_TEMP:
            temp_ecd = data[i]; break;
        case REG::NOW_CURRENT_L:
            cur_ecd = static_cast<int16_t>(data[i + 1] << 8 | data[i]); break;

        // case REG::ASYNCHRONOUS_WRITE:
            // usart_printf("[0x%02X] asynchronous write: 0x%02X\n", ID, data[i]); break;
        // case REG::STATUS:
            // usart_printf("[0x%02X] status: 0x%02X\n", ID, data[i]); break;
        // case REG::IS_MOVING:
            // usart_printf("[0x%02X] is moving: 0x%02X\n", ID, data[i]); break;

        default: break;
        }
    }
    read_reg_l = 0xFF; read_reg_h = 0x00;
    return CALLBACK_TYPE::STATE_PARAMS;
}

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


void cMotorSts::AddReadReg(const REG reg)
{
    const auto addr = static_cast<uint8_t>(reg);
    if (addr < read_reg_l) read_reg_l = addr;
    if (addr > read_reg_h) read_reg_h = addr;
}

void cMotorSts::AddReadRangeByCount(const REG start, const uint8_t count)
{
    if (count == 0) return;

    const auto start_addr = static_cast<uint8_t>(start);
    const uint8_t end_addr = start_addr + count - 1;
    if (start_addr < read_reg_l) read_reg_l = start_addr;
    if (end_addr   > read_reg_h) read_reg_h = end_addr;
}

void cMotorSts::AddReadRange(const REG start, const REG end)
{
    const auto start_addr = static_cast<uint8_t>(start);
    const auto end_addr = static_cast<uint8_t>(end);
    if (start_addr > end_addr) return;
    if (start_addr < read_reg_l) read_reg_l = start_addr;
    if (end_addr   > read_reg_h) read_reg_h = end_addr;
}

void cMotorSts::TransmitReadCommand() const
{
    if (read_reg_h < read_reg_l) return;
    uart10_tx_buffer[1] = uart10_tx_buffer[0] = 0xFF;    // TxHeader
    uart10_tx_buffer[2] = ID;                            // ID
    uart10_tx_buffer[3] = 0x04;                          // FrameLength
    uart10_tx_buffer[4] = 0x02;                          // Command
    uart10_tx_buffer[5] = read_reg_l;                    // RegStart
    uart10_tx_buffer[6] = read_reg_h - read_reg_l + 1;   // RegLength

    const uint8_t check_sum = ~(uart10_tx_buffer[2] +
        uart10_tx_buffer[3] +
        uart10_tx_buffer[4] +
        uart10_tx_buffer[5] +
        uart10_tx_buffer[6]);
    uart10_tx_buffer[7] = check_sum;                     // CheckSum
    HAL_UART_Transmit_DMA(&huart10, uart10_tx_buffer, 0x08);
}

void cMotorSts::TransmitWriteCommand(const REG reg, int16_t value) const
{
    const auto is_16_bit_reg = Is16BitWriteReg(reg);
    if (!is_16_bit_reg && (value & 0xFF00) != 0) return; // 传参错误不处理
    if ((value & 0x8000) != 0) value = static_cast<int16_t>(-(value & 0x7FFF)); // ccb飞特写你协议的有木了

    uart10_tx_buffer[1] = uart10_tx_buffer[0] = 0xFF;    // TxHeader
    uart10_tx_buffer[2] = ID;                            // ID
    uart10_tx_buffer[3] = is_16_bit_reg ? 0x05 : 0x04;   // FrameLength
    uart10_tx_buffer[4] = 0x03;                          // Command
    uart10_tx_buffer[5] = static_cast<uint8_t>(reg);     // RegStart
    uart10_tx_buffer[6] = value;                         // SetValue

    if (!is_16_bit_reg)
    {
        const uint8_t check_sum = ~(uart10_tx_buffer[2] +
            uart10_tx_buffer[3] +
            uart10_tx_buffer[4] +
            uart10_tx_buffer[5] +
            uart10_tx_buffer[6]);
        uart10_tx_buffer[7] = check_sum;                 // CheckSum
        HAL_UART_Transmit_DMA(&huart10, uart10_tx_buffer, 0x08);
    }
    else
    {
        uart10_tx_buffer[7] = value >> 8;
        const uint8_t check_sum = ~(uart10_tx_buffer[2] +
            uart10_tx_buffer[3] +
            uart10_tx_buffer[4] +
            uart10_tx_buffer[5] +
            uart10_tx_buffer[6] +
            uart10_tx_buffer[7]);
        uart10_tx_buffer[8] = check_sum;                 // CheckSum
        HAL_UART_Transmit_DMA(&huart10, uart10_tx_buffer, 0x09);
    }

}

