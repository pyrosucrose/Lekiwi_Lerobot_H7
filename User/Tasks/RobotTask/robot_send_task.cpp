//
// Created by Glucose_carbide on 25-8-8.
//

// =============================== 引入头文件 ===============================
#include "robot_send_task.hpp"
#include "config.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "remote.hpp"
#include "robot.hpp"
#include "usartio.hpp"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ==================================

// =============================== 函数实现 ===============================

void RobotSendTask(void *pv)
{
    portTickType current_time;
    while (true)
    {
        current_time = xTaskGetTickCount();         //获取当前tick（时间）
        // usart_printf("2\n");
        static bool read_motor_info = false;
        read_motor_info = !read_motor_info;
        if (read_motor_info)
        {
            static uint8_t num = 0;
            num += 1; num %= 9;
            switch (num)
            {
            case 0: robot_lekiwi.arm.motors[0].AddReadRange(cMotorSts::REG::NOW_POS_L, cMotorSts::REG::NOW_TEMP); robot_lekiwi.arm.motors[0].TransmitReadCommand(); break;
            case 1: robot_lekiwi.arm.motors[1].AddReadRange(cMotorSts::REG::NOW_POS_L, cMotorSts::REG::NOW_TEMP); robot_lekiwi.arm.motors[1].TransmitReadCommand(); break;
            case 2: robot_lekiwi.arm.motors[2].AddReadRange(cMotorSts::REG::NOW_POS_L, cMotorSts::REG::NOW_TEMP); robot_lekiwi.arm.motors[2].TransmitReadCommand(); break;
            case 3: robot_lekiwi.arm.motors[3].AddReadRange(cMotorSts::REG::NOW_POS_L, cMotorSts::REG::NOW_TEMP); robot_lekiwi.arm.motors[3].TransmitReadCommand(); break;
            case 4: robot_lekiwi.arm.motors[4].AddReadRange(cMotorSts::REG::NOW_POS_L, cMotorSts::REG::NOW_TEMP); robot_lekiwi.arm.motors[4].TransmitReadCommand(); break;
            case 5: robot_lekiwi.arm.motors[5].AddReadRange(cMotorSts::REG::NOW_POS_L, cMotorSts::REG::NOW_TEMP); robot_lekiwi.arm.motors[5].TransmitReadCommand(); break;
            case 6: robot_lekiwi.chassis.motors[0].AddReadRange(cMotorSts::REG::NOW_POS_L, cMotorSts::REG::NOW_TEMP); robot_lekiwi.chassis.motors[0].TransmitReadCommand(); break;
            case 7: robot_lekiwi.chassis.motors[1].AddReadRange(cMotorSts::REG::NOW_POS_L, cMotorSts::REG::NOW_TEMP); robot_lekiwi.chassis.motors[1].TransmitReadCommand(); break;
            case 8: robot_lekiwi.chassis.motors[2].AddReadRange(cMotorSts::REG::NOW_POS_L, cMotorSts::REG::NOW_TEMP); robot_lekiwi.chassis.motors[2].TransmitReadCommand(); break;
            default: break;
            }
        }
        else
        {
            static uint8_t num = 0;
            num += 1; num %= 9;
            switch (num)
            {
            case 0:
                // if (rc_data.IsRcOnline())
                    // robot_lekiwi.arm.motors[0].TransmitWriteCommand(cMotorSts::REG::TARGET_POSITION_L, 2048);
                // else
                robot_lekiwi.arm.motors[0].TransmitWriteCommand(cMotorSts::REG::TORQUE_SWITCH, 2);
                break;
            case 1:
                if (rc_data.IsRcOnline())
                    robot_lekiwi.arm.motors[1].TransmitWriteCommand(cMotorSts::REG::TARGET_POSITION_L, static_cast<int16_t>(robot_lekiwi.arm.target_pos_ecd[0]));
                else
                    robot_lekiwi.arm.motors[1].TransmitWriteCommand(cMotorSts::REG::TORQUE_SWITCH, 2);
                break;
            case 2:
                if (rc_data.IsRcOnline())
                    robot_lekiwi.arm.motors[2].TransmitWriteCommand(cMotorSts::REG::TARGET_POSITION_L, static_cast<int16_t>(robot_lekiwi.arm.target_pos_ecd[1]));
                else
                    robot_lekiwi.arm.motors[2].TransmitWriteCommand(cMotorSts::REG::TORQUE_SWITCH, 2);
                break;
            case 3:
                if (rc_data.IsRcOnline())
                    robot_lekiwi.arm.motors[3].TransmitWriteCommand(cMotorSts::REG::TARGET_POSITION_L, static_cast<int16_t>(robot_lekiwi.arm.target_pos_ecd[2]));
                else
                    robot_lekiwi.arm.motors[3].TransmitWriteCommand(cMotorSts::REG::TORQUE_SWITCH, 2);
                break;
            case 4:
                if (rc_data.IsRcOnline())
                    robot_lekiwi.arm.motors[4].TransmitWriteCommand(cMotorSts::REG::TARGET_POSITION_L, static_cast<int16_t>(robot_lekiwi.arm.target_pos_ecd[3]));
                else
                    robot_lekiwi.arm.motors[4].TransmitWriteCommand(cMotorSts::REG::TORQUE_SWITCH, 2);
                break;
            case 5:
                if (rc_data.IsRcOnline())
                    robot_lekiwi.arm.motors[5].TransmitWriteCommand(cMotorSts::REG::TARGET_POSITION_L, static_cast<int16_t>(robot_lekiwi.arm.target_pos_ecd[4]));
                else
                    robot_lekiwi.arm.motors[5].TransmitWriteCommand(cMotorSts::REG::TORQUE_SWITCH, 2);
                break;
            case 6:
                if (rc_data.IsRcOnline())
                    robot_lekiwi.chassis.motors[0].TransmitWriteCommand(cMotorSts::REG::TARGET_SPEED_L, static_cast<int16_t>(-robot_lekiwi.chassis.target_wheel_speed[0]));
                else
                    robot_lekiwi.chassis.motors[0].TransmitWriteCommand(cMotorSts::REG::TORQUE_SWITCH, 0);
                break;
            case 7:
                if (rc_data.IsRcOnline())
                    robot_lekiwi.chassis.motors[1].TransmitWriteCommand(cMotorSts::REG::TARGET_SPEED_L, static_cast<int16_t>(-robot_lekiwi.chassis.target_wheel_speed[1]));
                else
                    robot_lekiwi.chassis.motors[1].TransmitWriteCommand(cMotorSts::REG::TORQUE_SWITCH, 0);
                break;
            case 8:
                if (rc_data.IsRcOnline())
                    robot_lekiwi.chassis.motors[2].TransmitWriteCommand(cMotorSts::REG::TARGET_SPEED_L, static_cast<int16_t>(-robot_lekiwi.chassis.target_wheel_speed[2]));
                else
                    robot_lekiwi.chassis.motors[2].TransmitWriteCommand(cMotorSts::REG::TORQUE_SWITCH, 0);
                break;
            default:
                break;
            }
        }
        vTaskDelayUntil(&current_time, pdMS_TO_TICKS(TASK_ROBOT_SEND_TASK_PERIOD));       //让任务以 5 毫秒的周期运行
    }
}