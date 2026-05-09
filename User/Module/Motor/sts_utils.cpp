//
// Created by Glucose_carbide on 2026-05-02.
//

#include "motor_sts.hpp"

#include "crash.hpp"
#include "delay.hpp"
#include "usartio.hpp"

/**
 * @brief   拼接返回数据
 * @param   L: 低位数据(数组前)
 * @param   H: 高位数据(数组后)
 * @retval  拼接后数据，为int16
 */
int16_t MotorSts::PackStsData(const uint8_t L, const uint8_t H, const uint8_t s)
{
    const auto v = static_cast<int16_t>(H << 8 | L);
    return v & (1 << s) ? static_cast<int16_t>(-(v & ((1 << s) - 1))) : static_cast<int16_t>(v & ((1 << s) - 1));
}

/**
 * @brief   处理STS特有的(呕)数据格式
 * @param   d: 传入的数据
 * @param   s: 该数据的符号位，默认为最高位15
 * @retval  转化为STS格式后的数据(bit s 表示正负号，低位是绝对值)
 * @note    通常只会对负数有影响
 * @note    虽然声明传入的是uint16，但传int16也是一样的
 * @note    想出这样处理(u)int16的家里清明节指定能多出些什么来
 */
uint16_t MotorSts::ConvertStsData(const uint16_t d, const uint8_t s)
{
    return d & (1 << s) ? (abs(static_cast<int16_t>(d)) & ((1 << s) - 1)) | (1 << s) : d & ((1 << s) - 1);
}

/**
 * @brief   表示传入的寄存器地址是否是某个16bit数据的低位寄存器
 * @param   r:寄存器地址
 * @retval  是不是
 */
bool MotorSts::IsLowByteReg(const REG r)
{
    return r == REG::TARGET_POSITION_L ||
           r == REG::TARGET_SPEED_L ||
           r == REG::MIN_ANGLE_LIMIT_L ||
           r == REG::MAX_ANGLE_LIMIT_L ||
           r == REG::MAX_TORQUE_L ||
           r == REG::PROTECTION_CURRENT_L ||
           r == REG::POSITION_CORRECTION_L ||
           r == REG::MOVING_TIME_L ||
           r == REG::TORQUE_LIMIT_L;
}

/**
 * @brief   电机的构造函数
 * @details 新建电机后会将其添加至静态数组中以便批量操作
 * @param   ID          :该电机ID
 * @param   zero_point  :电机零点(用户希望电机所在的零点)
 * @param   min_angle   :电机所有允许的姿态下，编码器最小值
 * @param   max_angle   :电机所有允许的姿态下，编码器最大值
 * @param   reversed    :是否反转，和安装方式相关(若用户期望的电机方向和点击实际方向相反就设为true)
 * @param   mode        :电机模式(位控/速控/PWM/步进模式)
 * @note    若从这进了Crash那可能是电机ID冲突或是注册的电机过多导致超出MAX_MOTORS_COUNT，需要修正或修改相关配置
 * @warning 对于重复ID的处理能力有限，用户必须保证没用重复ID的电机被写入！
 */
MotorSts::MotorSts(const uint8_t ID, const uint16_t zero_point, const uint16_t min_angle, const uint16_t max_angle, const bool reversed, const Mode mode) :
    ID_(ID), is_reversed_(reversed), mode_(mode), zero_point_ecd_(zero_point),
    min_pos_ecd_(min_angle), soft_min_pos_ecd_(static_cast<int16_t>(reversed ? zero_point - max_angle : min_angle - zero_point)),
    max_pos_ecd_(max_angle), soft_max_pos_ecd_(static_cast<int16_t>(reversed ? zero_point - min_angle : max_angle - zero_point))
{
    if (motors_count_ < MAX_MOTORS_COUNT)
    {
        if (motors_idx_[ID] != 0x00 && motors_idx_[ID] != Special::ILLEGAL_ID)  // 没辙，否则idx_必须手动写255个0xFF进去
            // 你是不是注册了ID相同的电机 o_O
            Crash();
        if constexpr (!USE_MOTOR_POS_LIMIT)
            if (min_angle >= max_angle)
                // 最大角度和最小角度都是绝对的编码器值，不是软件限制值，所以小的必须是小的
                Crash();
        motors_[motors_count_] = this;
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

/**
 * @brief   更新所有电机数据
 * @details 对于有新数据返回的电机，调用UnpackData来解析
 * @details 对所有电机更新计时器，若超时则认为电机掉线，重置其状态机
 * @note    该函数应该在主循环或任务中由用户手动调用
 */
void MotorSts::UpdateAll()
{
    for (uint8_t i = 0; i < motors_count_; i++)
    {
        const auto tick_now = HAL_GetTick();
        if (motors_[i]->received_pack_)
            motors_[i]->UnpackData();
        if (tick_now - motors_[i]->last_ack_tick_ >= TIMEOUT_TICK)
            motors_[i]->Reload(tick_now);
        if (tick_now - last_ack_all_ >= TIMEOUT_TICK)
        {
            connected_ = false;
            last_ack_all_ = tick_now;
            Uart10_Restart();
        }
    }
}

/**
 * @brief   解析电机接收数据
 * @details 解包返回帧，并设置标志位
 * @note    用户可选择开启DEBUG_MODE来查看接收数据
 * @warning 用户必须确保若读取了最大/小角度时必须读取另一个，否则程序行为无法预料
 */
void MotorSts::UnpackData()
{
    is_unpacking_ = true;
    status_ = rx_buffer_[1];

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
            soft_pos_ecd_ = static_cast<int16_t>(is_reversed_ ? zero_point_ecd_ - pos_ecd_ : -zero_point_ecd_ + pos_ecd_);
            break;
        case REG::NOW_SPEED_L:
            vel_ecd_  = PackStsData(rx_buffer_[i], rx_buffer_[i + 1]);
            soft_vel_ecd_ = static_cast<int16_t>(is_reversed_ ? -vel_ecd_ : vel_ecd_);
            break;
        case REG::NOW_LOAD_L:
            load_ecd_ = PackStsData(rx_buffer_[i], rx_buffer_[i + 1], 10);
            soft_load_ecd_ =  static_cast<int16_t>(is_reversed_ ? -load_ecd_ : load_ecd_);
            break;
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
 * @brief 重置电机状态机，在电机超时时调用
 */
void MotorSts::Reload(const uint32_t tick_now)
{
    is_online_ = false;
    is_param_set_ = false;
    received_pack_ = false;
    in_use_ = false;
    is_unpacking_ = false;
    last_ack_tick_ = tick_now;
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
 * @brief   设置目标角度
 * @param   t: 目标角度(编码器刻度)
 * @note    该功能设置的角度是相对于用户定义的零点和方向的，模块暂未开放绝对位置设置
 */
void MotorSts::SetSoftTargetPos_Ecd(const int16_t t)
{
    is_param_set_ = true;
    target_pos_ecd_ =
        utils::Clamp(static_cast<uint16_t>(is_reversed_ ? zero_point_ecd_ - t : zero_point_ecd_ + t), min_pos_ecd_, max_pos_ecd_);
    soft_target_pos_ecd_ =
        utils::Clamp(t, soft_min_pos_ecd_, soft_max_pos_ecd_);
}
