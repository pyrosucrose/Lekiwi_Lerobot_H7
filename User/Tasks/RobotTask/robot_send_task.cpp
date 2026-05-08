//
// Created by Glucose_carbide on 2026-05-02.
//

#include "robot_send_task.hpp"
#include "config.hpp"
#include "delay.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "robot.hpp"
#include "usartio.hpp"

void RobotSendTask(void *pv)
{
    portTickType current_time = xTaskGetTickCount();
    while (true)
    {
        RobotLekiwi::SendControl();
        vTaskDelayUntil(&current_time, pdMS_TO_TICKS(TASK_ROBOT_SEND_TASK_PERIOD));
    }
}