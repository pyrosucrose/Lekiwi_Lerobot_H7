//
// Created by Glucose_carbide on 25-8-8.
//

// =============================== 引入头文件 ===============================
#include "robot_send_task.hpp"
#include "config.hpp"
#include "delay.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "robot.hpp"
#include "usartio.hpp"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ==================================

// =============================== 函数实现 ===============================

void RobotSendTask(void *pv)
{
    portTickType current_time = xTaskGetTickCount();
    // MotorSts motor(1,2048,1000,3000,false);
    while (true)
    {
        // auto s = Delay::GetTimeStamp();
        robot_lekiwi.SendControl();
        // usart_printf("%d,%d,%d,%d,%d\n",
        //     MotorSts::motors_[5]->zero_point_ecd_,
        //     MotorSts::motors_[5]->min_pos_ecd_,
        //     MotorSts::motors_[5]->max_pos_ecd_,
        //     MotorSts::motors_[5]->soft_min_pos_ecd_,
        //     MotorSts::motors_[5]->soft_max_pos_ecd_
        //     );
        // MotorSts::ReadAll(MotorSts::REG::NOW_POS_L, MotorSts::REG::NOW_TEMP);
        // usart_printf("%f\n",Delay::CalculateInterval_us(s));
        // usart_printf("%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
        //     MotorSts::motors_[0]->pos_ecd_,
        //     MotorSts::motors_[1]->pos_ecd_,
        //     MotorSts::motors_[2]->pos_ecd_,
        //     MotorSts::motors_[3]->pos_ecd_,
        //     MotorSts::motors_[4]->pos_ecd_,
        //     MotorSts::motors_[5]->pos_ecd_,
        //     MotorSts::motors_[6]->pos_ecd_,
        //     MotorSts::motors_[7]->pos_ecd_,
        //     MotorSts::motors_[8]->pos_ecd_
        //     );

        // usart_printf("%d,%d,%d,%d,%d,%d\n",
        //     robot_lekiwi.arm.motors[0].pos_ecd_,
        //     robot_lekiwi.arm.motors[1].pos_ecd_,
        //     robot_lekiwi.arm.motors[2].pos_ecd_,
        //     robot_lekiwi.arm.motors[3].pos_ecd_,
        //     robot_lekiwi.arm.motors[4].pos_ecd_,
        //     robot_lekiwi.arm.motors[5].pos_ecd_
        //     );

        // usart_printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
        //     robot_lekiwi.arm.motors[1].soft_min_pos_ecd_,
        //     robot_lekiwi.arm.motors[1].soft_max_pos_ecd_,
        //     robot_lekiwi.arm.motors[2].soft_min_pos_ecd_,
        //     robot_lekiwi.arm.motors[2].soft_max_pos_ecd_,
        //     robot_lekiwi.arm.motors[3].soft_min_pos_ecd_,
        //     robot_lekiwi.arm.motors[3].soft_max_pos_ecd_,
        //     robot_lekiwi.arm.motors[4].soft_min_pos_ecd_,
        //     robot_lekiwi.arm.motors[4].soft_max_pos_ecd_,
        //     robot_lekiwi.arm.motors[5].soft_min_pos_ecd_,
        //     robot_lekiwi.arm.motors[5].soft_max_pos_ecd_
        //     );
        vTaskDelayUntil(&current_time, pdMS_TO_TICKS(TASK_ROBOT_SEND_TASK_PERIOD));
    }
}