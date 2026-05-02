//
// Created by Glucose_carbide on 2026-05-02.
//

#include "chassis.hpp"
#include <cmath>
#include "typedef.hpp"
#include "config.hpp"
#include "remote.hpp"

void LekiwiChassis::GetDataFromRc()
{
    if (rc_data.GetRcSwitchA() == HIGH && rc_data.GetRcSwitchB() == HIGH)
    {
        target_speed[X] = rc_data.GetRcLeftHorizontal() * 1800.0f;
        target_speed[Y] = rc_data.GetRcLeftVertical() * 1800.0f;
        target_speed[Z] = rc_data.GetRcRightHorizontal() * 1800.0f;

        // target_speed[X] = rc_data.GetRcLeftHorizontal() * 1800.0f;
        // target_speed[Y] = rc_data.GetRcLeftVertical() * 1800.0f;
        // target_speed[Z] = rc_data.GetRcRightHorizontal() * 1800.0f;
    }
    else
    {
        target_speed[X] = 0;
        target_speed[Y] = 0;
        target_speed[Z] = 0;
        // target_speed[Z] = rc_data.GetRcLeftHorizontal() * 1800.0f;
    }
}

void LekiwiChassis::SolveWheelSpeed()
{
    target_wheel_speed[0] = target_speed[Z] - target_speed[X];
    target_wheel_speed[1] = target_speed[Z] + target_speed[X] * 0.5f - target_speed[Y] * (sqrtf(3.0f) / 2);
    target_wheel_speed[2] = target_speed[Z] + target_speed[X] * 0.5f + target_speed[Y] * (sqrtf(3.0f) / 2);
    for (uint8_t i = 0; i < 3; i++)
        motors[i].SetSoftTargetVel_Ecd(static_cast<int16_t>(target_wheel_speed[i]));
}

void LekiwiChassis::TransmitBusControlCmd()
{
    uint8_t idx = 0;
    uart_sts_tx_buffer[idx++] = 0xFF;
    uart_sts_tx_buffer[idx++] = 0xFF;
    uart_sts_tx_buffer[idx++] = MotorSts::Special::MASTER_ID;
    uart_sts_tx_buffer[idx++] = MotorSts::Special::DUMMY;
    uart_sts_tx_buffer[idx++] = MotorSts::Command::SYN_WRITE;
    uart_sts_tx_buffer[idx++] = static_cast<uint8_t>(MotorSts::REG::TARGET_SPEED_L);
    uart_sts_tx_buffer[idx++] = 0x02;

    for (const auto& motor : motors)
    {
        const uint16_t val = MotorSts::ConvertStsData(motor.GetHardTargetVel_Ecd());

        uart_sts_tx_buffer[idx++] = motor.GetID();
        uart_sts_tx_buffer[idx++] = val;
        uart_sts_tx_buffer[idx++] = val >> 8;
    }
    uart_sts_tx_buffer[3] = idx - 3;                          // FrameLength

    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < idx; i++)
        check_sum += uart_sts_tx_buffer[i];

    uart_sts_tx_buffer[idx++] = ~check_sum;

    HAL_UART_Transmit_DMA(&huart_sts, uart_sts_tx_buffer, idx);
}

void LekiwiChassis::DisableAll()
{
    uint8_t idx = 0;
    uart_sts_tx_buffer[idx++] = 0xFF;
    uart_sts_tx_buffer[idx++] = 0xFF;
    uart_sts_tx_buffer[idx++] = MotorSts::Special::MASTER_ID;
    uart_sts_tx_buffer[idx++] = MotorSts::Special::DUMMY;
    uart_sts_tx_buffer[idx++] = MotorSts::Command::SYN_WRITE;
    uart_sts_tx_buffer[idx++] = static_cast<uint8_t>(MotorSts::REG::TORQUE_SWITCH);
    uart_sts_tx_buffer[idx++] = 0x01;

    for (const auto& motor : motors)
    {
        uart_sts_tx_buffer[idx++] = motor.GetID();
        uart_sts_tx_buffer[idx++] = MotorSts::TorqueSwitch::OFF;
    }
    uart_sts_tx_buffer[3] = idx - 3;                          // FrameLength

    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < idx; i++)
        check_sum += uart_sts_tx_buffer[i];

    uart_sts_tx_buffer[idx++] = ~check_sum;
    // 发送
    HAL_UART_Transmit_DMA(&huart_sts, uart_sts_tx_buffer, idx);
}

void LekiwiChassis::ControlLoop()
{
    if (rc_data.IsRcOnline())
    {
        GetDataFromRc();
        SolveWheelSpeed();
    }
}