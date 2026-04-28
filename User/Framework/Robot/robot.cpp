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
    // chassis.RxCallback(data);
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

void cRobot::SendControl()
{
    static bool read_motor_info = false;
    read_motor_info = !read_motor_info;

    if (read_motor_info)
    {
        cMotorSts::ReadAll(cMotorSts::REG::LOCK, cMotorSts::REG::NOW_TEMP);
    }
    else
    {
        if (rc_data.IsRcOnline() && rc_data.GetRcSwitchC() != UP)
        {
            cMotorSts::ControlAll();
        }
        else
        {
            static bool to_arm = true; to_arm = !to_arm;
            if (to_arm)
                arm.DisableAll();
            else
                chassis.DisableAll();
        }
    }

    // if (read_motor_info)
    // {
    //     static uint8_t num = 0;
    //     num += 1; num %= 9;
    //     switch (num)
    //     {
    //     case 0: arm.motors[0].AddReadRange(cMotorSts::REG::RETURN_DELAY, cMotorSts::REG::RETURN_DELAY); arm.motors[0].TransmitReadCommand(); break;
    //     case 1: arm.motors[1].AddReadRange(cMotorSts::REG::RETURN_DELAY, cMotorSts::REG::RETURN_DELAY); arm.motors[1].TransmitReadCommand(); break;
    //     case 2: arm.motors[2].AddReadRange(cMotorSts::REG::RETURN_DELAY, cMotorSts::REG::RETURN_DELAY); arm.motors[2].TransmitReadCommand(); break;
    //     case 3: arm.motors[3].AddReadRange(cMotorSts::REG::RETURN_DELAY, cMotorSts::REG::RETURN_DELAY); arm.motors[3].TransmitReadCommand(); break;
    //     case 4: arm.motors[4].AddReadRange(cMotorSts::REG::RETURN_DELAY, cMotorSts::REG::RETURN_DELAY); arm.motors[4].TransmitReadCommand(); break;
    //     case 5: arm.motors[5].AddReadRange(cMotorSts::REG::RETURN_DELAY, cMotorSts::REG::RETURN_DELAY); arm.motors[5].TransmitReadCommand(); break;
    //     case 6: chassis.motors[0].AddReadRange(cMotorSts::REG::RETURN_DELAY, cMotorSts::REG::RETURN_DELAY); chassis.motors[0].TransmitReadCommand(); break;
    //     case 7: chassis.motors[1].AddReadRange(cMotorSts::REG::RETURN_DELAY, cMotorSts::REG::RETURN_DELAY); chassis.motors[1].TransmitReadCommand(); break;
    //     case 8: chassis.motors[2].AddReadRange(cMotorSts::REG::RETURN_DELAY, cMotorSts::REG::RETURN_DELAY); chassis.motors[2].TransmitReadCommand(); break;
    //     default: break;
    //     }
    // }
    // else
    // {
    //     static uint8_t num = 0;
    //     num += 1; num %= 9;
    //     if (rc_data.IsRcOnline() && rc_data.GetRcSwitchC() != UP)
    //     {
    //         switch (num)
    //         {
    //         case 0: arm.motors[0].TransmitWriteCommand(cMotorSts::REG::RETURN_DELAY, 5); break;
    //         case 1: arm.motors[1].TransmitWriteCommand(cMotorSts::REG::RETURN_DELAY, 5); break;
    //         case 2: arm.motors[2].TransmitWriteCommand(cMotorSts::REG::RETURN_DELAY, 5); break;
    //         case 3: arm.motors[3].TransmitWriteCommand(cMotorSts::REG::RETURN_DELAY, 5); break;
    //         case 4: arm.motors[4].TransmitWriteCommand(cMotorSts::REG::RETURN_DELAY, 5); break;
    //         case 5: arm.motors[5].TransmitWriteCommand(cMotorSts::REG::RETURN_DELAY, 5); break;
    //         case 6: chassis.motors[0].TransmitWriteCommand(cMotorSts::REG::RETURN_DELAY, 5); break;
    //         case 7: chassis.motors[1].TransmitWriteCommand(cMotorSts::REG::RETURN_DELAY, 5); break;
    //         case 8: chassis.motors[2].TransmitWriteCommand(cMotorSts::REG::RETURN_DELAY, 5); break;
    //         default: break;
    //         }
    //     }
    //     else
    //     {
    //         // 遥控器离线时的逻辑
    //         switch (num)
    //         {
    //         case 0: arm.motors[0].TransmitWriteCommand(cMotorSts::REG::ACCELERATION, 254); break;
    //         case 1: arm.motors[1].TransmitWriteCommand(cMotorSts::REG::ACCELERATION, 254); break;
    //         case 2: arm.motors[2].TransmitWriteCommand(cMotorSts::REG::ACCELERATION, 254); break;
    //         case 3: arm.motors[3].TransmitWriteCommand(cMotorSts::REG::ACCELERATION, 254); break;
    //         case 4: arm.motors[4].TransmitWriteCommand(cMotorSts::REG::ACCELERATION, 254); break;
    //         case 5: arm.motors[5].TransmitWriteCommand(cMotorSts::REG::ACCELERATION, 254); break;
    //         case 6: chassis.motors[0].TransmitWriteCommand(cMotorSts::REG::ACCELERATION, 254); break;
    //         case 7: chassis.motors[1].TransmitWriteCommand(cMotorSts::REG::ACCELERATION, 254); break;
    //         case 8: chassis.motors[2].TransmitWriteCommand(cMotorSts::REG::ACCELERATION, 254); break;
    //         default: break;
    //         }
    //     }
    // }
    // usart_printf("%d,%d\n",chassis.motors[0].pos_ecd,chassis.motors[0].vel_ecd);
}
