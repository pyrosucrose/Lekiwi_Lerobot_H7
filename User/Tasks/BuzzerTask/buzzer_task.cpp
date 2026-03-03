//
// Created by Glucose_carbide on 25-8-8.
//

// =============================== 引入头文件 ===============================
#include "buzzer_task.hpp"
#include "config.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "buzzer.hpp"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ==================================

// =============================== 函数实现 ===============================

void BuzzerTask(void *pv)
{
    portTickType current_time;
    while (true)
    {
        current_time = xTaskGetTickCount();         //获取当前tick（时间）

        vTaskDelayUntil(&current_time, pdMS_TO_TICKS(TASK_BUZZER_TASK_PERIOD));       //让任务以 5 毫秒的周期运行
    }
}