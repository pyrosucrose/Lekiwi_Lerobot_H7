//
// Created by Glucose_carbide on 2026-05-02.
//

#include "robot_task.hpp"
#include "config.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "robot.hpp"

void RobotTask(void *pv)
{
    portTickType current_time = xTaskGetTickCount();
    while (true)
    {
        robot_lekiwi.ControlLoop();
        vTaskDelayUntil(&current_time, pdMS_TO_TICKS(TASK_ROBOT_TASK_PERIOD));       //让任务以 5 毫秒的周期运行
    }
}