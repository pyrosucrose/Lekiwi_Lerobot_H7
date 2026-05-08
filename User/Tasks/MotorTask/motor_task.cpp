//
// Created by Glucose_carbide on 2026-05-02.
//

#include "motor_task.hpp"
#include "config.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "chassis.hpp"

void MotorTask(void *pv)
{
    portTickType current_time = xTaskGetTickCount();
    while (true)
    {
        MotorSts::UpdateAll();
        vTaskDelayUntil(&current_time, pdMS_TO_TICKS(TASK_MOTOR_TASK_PERIOD));
    }
}