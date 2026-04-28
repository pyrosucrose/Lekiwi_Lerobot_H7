//
// Created by Glucose_carbide on 25-8-8.
//

// =============================== 引入头文件 ===============================
#include "robot_send_task.hpp"
#include "config.hpp"
#include "delay.hpp"
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
        cMotorSts::UnpackAll();
        robot_lekiwi.SendControl();
        // usart_printf("%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
        //     cMotorSts::motors_[0]->pos_ecd,
        //     cMotorSts::motors_[1]->pos_ecd,
        //     cMotorSts::motors_[2]->pos_ecd,
        //     cMotorSts::motors_[3]->pos_ecd,
        //     cMotorSts::motors_[4]->pos_ecd,
        //     cMotorSts::motors_[5]->pos_ecd,
        //     cMotorSts::motors_[6]->pos_ecd,
        //     cMotorSts::motors_[7]->pos_ecd,
        //     cMotorSts::motors_[8]->pos_ecd
        //     );
        vTaskDelayUntil(&current_time, pdMS_TO_TICKS(TASK_ROBOT_SEND_TASK_PERIOD));       //让任务以 5 毫秒的周期运行
    }
}