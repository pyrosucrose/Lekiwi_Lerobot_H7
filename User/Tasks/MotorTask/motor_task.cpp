//
// Created by Glucose_carbide on 2026-04-29.
//

// =============================== 引入头文件 ===============================
#include "motor_task.hpp"
#include "config.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "usartio.hpp"
#include "chassis.hpp"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ==================================

// =============================== 函数实现 ===============================

void MotorTask(void *pv)
{
    portTickType current_time = xTaskGetTickCount();
    while (true)
    {
        // usart_printf("%d",MotorSts::motors_[0]->received_pack_);
        MotorSts::UnpackAll();
        // usart_printf("%d\n",MotorSts::motors_[0]->received_pack_);
        vTaskDelayUntil(&current_time, pdMS_TO_TICKS(TASK_MOTOR_TASK_PERIOD));
    }
}