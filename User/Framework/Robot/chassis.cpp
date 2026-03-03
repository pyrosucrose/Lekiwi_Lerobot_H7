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

bool cChassis::RxCallback(const uint8_t* data)
{
    for (auto& motor : motors)
        if (motor.RxCallback(data) == cMotorSts::CALLBACK_TYPE::STATE_PARAMS)
            return true;
    return false;
}

void cChassis::GetDataFromRc()
{
    if (rc_data.GetRcSwitchA() == HIGH)
    {
        target_speed[X] = rc_data.GetRcLeftHorizontal() * 1800.0f;
        target_speed[Y] = rc_data.GetRcLeftVertical() * 1800.0f;
        target_speed[Z] = rc_data.GetRcRightHorizontal() * 1800.0f;
    }
    else
    {
        target_speed[X] = 0;
        target_speed[Y] = 0;
        target_speed[Z] = rc_data.GetRcLeftHorizontal() * 1800.0f;
    }
}

void cChassis::SolveWheelSpeed()
{
    // ∵ 从下逆时针，012
    target_wheel_speed[0] = target_speed[Z] - target_speed[X];
    target_wheel_speed[1] = target_speed[Z] + target_speed[X] * 0.5f - target_speed[Y] * (sqrtf(3.0f) / 2);
    target_wheel_speed[2] = target_speed[Z] + target_speed[X] * 0.5f + target_speed[Y] * (sqrtf(3.0f) / 2);
}

void cChassis::TransmitBusControlCmd()
{
    uart10_tx_buffer[1] = uart10_tx_buffer[0] = 0xFF;
    uart10_tx_buffer[2] = 0xFE;
    uart10_tx_buffer[3] = 0x16;
    uart10_tx_buffer[4] = 0x83;
    uart10_tx_buffer[5] = static_cast<uint8_t>(cMotorSts::REG::TARGET_SPEED_L);
    uart10_tx_buffer[6] = 0x02;

    uint8_t idx = 7;
    for (const auto& motor : motors)
    {
        int16_t val = motor.target_vel;
        if ((val & 0x8000) != 0) val = static_cast<int16_t>(-(val & 0x7FFF));

        uart10_tx_buffer[idx] = motor.ID;
        uart10_tx_buffer[idx + 1] = val;
        uart10_tx_buffer[idx + 2] = val >> 8;

        idx += 3;
    }

    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < idx; i++)
        check_sum += uart10_tx_buffer[i];
    check_sum = ~check_sum;
    uart10_tx_buffer[idx] = check_sum;

    HAL_UART_Transmit_DMA(&huart10, uart10_tx_buffer, idx);
}

void cChassis::ControlLoop()
{
    GetDataFromRc();
    SolveWheelSpeed();
}