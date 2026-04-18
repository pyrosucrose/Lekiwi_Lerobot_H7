//
// Created by Glucose_carbide on 25-8-8.
//

// =============================== 引入头文件 ===============================
#include "robot.hpp"
#include "main.h"
#include "remote.hpp"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ==================================

// =============================== 函数实现 ===============================

void cRobot::RxCallback(const uint8_t* data)
{
    if (arm.RxCallback(data)) return;
    chassis.RxCallback(data);
}

void cRobot::ControlLoop()
{
    if (rc_data.IsRcOnline())
    // if (false)
    {
        chassis.ControlLoop();
        arm.ControlLoop();

        // arm.motors[0].TransmitWriteCommand(cMotorSts::REG::TARGET_POSITION_L, 2048); HAL_Delay(1);
        // arm.motors[1].TransmitWriteCommand(cMotorSts::REG::TARGET_POSITION_L, static_cast<int16_t>(arm.target_pos_ecd[0])); HAL_Delay(1);
        // arm.motors[2].TransmitWriteCommand(cMotorSts::REG::TARGET_POSITION_L, static_cast<int16_t>(arm.target_pos_ecd[1])); HAL_Delay(1);
        // arm.motors[3].TransmitWriteCommand(cMotorSts::REG::TARGET_POSITION_L, static_cast<int16_t>(arm.target_pos_ecd[2])); HAL_Delay(1);
        //
        // chassis.motors[0].TransmitWriteCommand(cMotorSts::REG::TARGET_SPEED_L, static_cast<int16_t>(-chassis.target_wheel_speed[0])); HAL_Delay(1);
        // chassis.motors[1].TransmitWriteCommand(cMotorSts::REG::TARGET_SPEED_L, static_cast<int16_t>(-chassis.target_wheel_speed[1])); HAL_Delay(1);
        // chassis.motors[2].TransmitWriteCommand(cMotorSts::REG::TARGET_SPEED_L, static_cast<int16_t>(-chassis.target_wheel_speed[2])); HAL_Delay(1);

    }
}

