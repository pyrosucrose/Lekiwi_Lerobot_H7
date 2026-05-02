//
// Created by Glucose_carbide on 2026-05-02.
//

#include "debug_task.hpp"
#include "config.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "usartio.hpp"
#include "chassis.hpp"

void DebugTask(void *pv)
{
    portTickType current_time = xTaskGetTickCount();
    while (true)
    {
        /* USER DEBUG CODE BEGIN */

        /* USER DEBUG CODE END  */
        vTaskDelayUntil(&current_time, pdMS_TO_TICKS(TASK_DEBUG_TASK_PERIOD));       //让任务以 5 毫秒的周期运行
    }
}