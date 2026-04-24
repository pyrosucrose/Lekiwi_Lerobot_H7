//
// Created by Glucose_carbide on 25-8-8.
//

// =============================== 引入头文件 ===============================
#include "robot_task.hpp"
#include "config.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "remote.hpp"
#include "robot.hpp"
#include "usartio.hpp"
#include "iwdg.h"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ==================================

// =============================== 函数实现 ===============================

void RobotTask(void *pv)
{
    portTickType current_time;
    while (true)
    {
        // usart_printf("1\n");
        current_time = xTaskGetTickCount();         //获取当前tick（时间）
        robot_lekiwi.ControlLoop();
        if (rc_data.IsRcOnline())
            HAL_IWDG_Refresh(&hiwdg1);

        // robot_lekiwi.arm.DeSolveArm(robot_lekiwi.arm.motors[1].pos_ecd, robot_lekiwi.arm.motors[2].pos_ecd, robot_lekiwi.arm.motors[3].pos_ecd);

        vTaskDelayUntil(&current_time, pdMS_TO_TICKS(TASK_ROBOT_TASK_PERIOD));       //让任务以 5 毫秒的周期运行
    }
}