//
// Created by Glucose_carbide on 25-8-8.
//

// =============================== 引入头文件 ===============================
#include "chassis.hpp"
#include <cmath>
#include "math.hpp"
#include "typedef.hpp"
#include "config.hpp"
#include "delay.hpp"
#include "remote.hpp"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ==================================

// =============================== 函数实现 ===============================
void cChassis::GetDataFromRc()
{
    if (rc_data.GetRcSwitchA() == HIGH && rc_data.GetRcSwitchB() == HIGH)
    {
        float x_tmp = rc_data.GetRcLeftHorizontal() * 1800.0f;
        x_tmp = x_tmp >= 100 ? x_tmp - 100 : x_tmp <= -100 ? x_tmp + 100 : 0;
        float y_tmp = rc_data.GetRcLeftVertical() * 1800.0f;
        y_tmp = y_tmp >= 100 ? y_tmp - 100 : y_tmp <= -100 ? y_tmp + 100 : 0;
        float z_tmp = rc_data.GetRcRightHorizontal() * 1800.0f;
        z_tmp = z_tmp >= 100 ? z_tmp - 100 : z_tmp <= -100 ? z_tmp + 100 : 0;
        target_speed[X] = x_tmp;
        target_speed[Y] = y_tmp;
        target_speed[Z] = z_tmp;

        // target_speed[X] = rc_data.GetRcLeftHorizontal() * 1800.0f;
        // target_speed[Y] = rc_data.GetRcLeftVertical() * 1800.0f;
        // target_speed[Z] = rc_data.GetRcRightHorizontal() * 1800.0f;
    }
    else
    {
        target_speed[X] = 0;
        target_speed[Y] = 0;

        float z_tmp = rc_data.GetRcLeftHorizontal() * 1800.0f;
        z_tmp = z_tmp >= 100 ? z_tmp - 100 : z_tmp <= -100 ? z_tmp + 100 : 0;
        target_speed[Z] = z_tmp;

        // target_speed[Z] = rc_data.GetRcLeftHorizontal() * 1800.0f;
    }
}

void cChassis::SolveWheelSpeed()
{
    target_wheel_speed[0] = target_speed[Z] - target_speed[X];
    target_wheel_speed[1] = target_speed[Z] + target_speed[X] * 0.5f - target_speed[Y] * (sqrtf(3.0f) / 2);
    target_wheel_speed[2] = target_speed[Z] + target_speed[X] * 0.5f + target_speed[Y] * (sqrtf(3.0f) / 2);
    for (uint8_t i = 0; i < 3; i++)
        motors[i].target_vel_ = static_cast<uint16_t>(static_cast<int16_t>(-target_wheel_speed[i]));
}

void cChassis::TransmitBusControlCmd()
{   // 帧头
    uint8_t idx = 0;
    uart10_tx_buffer[idx++] = 0xFF;
    uart10_tx_buffer[idx++] = 0xFF;
    uart10_tx_buffer[idx++] = 0xFE;
    uart10_tx_buffer[idx++] = 0x0D;
    uart10_tx_buffer[idx++] = 0x83;
    uart10_tx_buffer[idx++] = static_cast<uint8_t>(cMotorSts::REG::TARGET_SPEED_L);
    uart10_tx_buffer[idx++] = 0x02;
    // 命令
    for (const auto& motor : motors)
    {
        uint16_t val = motor.target_vel_;
        if ((val & 0x8000) != 0) val = static_cast<uint16_t>(-(val & 0x7FFF));

        uart10_tx_buffer[idx++] = motor.ID_;
        uart10_tx_buffer[idx++] = val;
        uart10_tx_buffer[idx++] = val >> 8;
    }
    // 校验和
    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < idx; i++)
        check_sum += uart10_tx_buffer[i];
    check_sum = ~check_sum;
    uart10_tx_buffer[idx++] = check_sum;
    // 发送
    HAL_UART_Transmit_DMA(&huart10, uart10_tx_buffer, idx);
}

void cChassis::DisableAll()
{   // 帧头
    uint8_t idx = 0;
    uart10_tx_buffer[idx++] = 0xFF;
    uart10_tx_buffer[idx++] = 0xFF;
    uart10_tx_buffer[idx++] = 0xFE;
    uart10_tx_buffer[idx++] = 0x0A;
    uart10_tx_buffer[idx++] = 0x83;
    uart10_tx_buffer[idx++] = static_cast<uint8_t>(cMotorSts::REG::TORQUE_SWITCH);
    uart10_tx_buffer[idx++] = 0x01;
    // 命令
    for (const auto& motor : motors)
    {
        uart10_tx_buffer[idx++] = motor.ID_;
        uart10_tx_buffer[idx++] = 0;
    }
    // 校验和
    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < idx; i++)
        check_sum += uart10_tx_buffer[i];
    // check_sum = ~check_sum;
    uart10_tx_buffer[idx++] = ~check_sum;
    // 发送
    HAL_UART_Transmit_DMA(&huart10, uart10_tx_buffer, idx);
}

void cChassis::ControlLoop()
{
    if (rc_data.IsRcOnline())
    {
        GetDataFromRc();
        SolveWheelSpeed();
    }
}