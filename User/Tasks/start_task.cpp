//
// Created by Glucose_carbide on 2026-05-02.
//

#include "start_task.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "crash.hpp"
#include "iwdg.h"
#include "remote.hpp"

#include "buzzer_task.hpp"
#include "debug_task.hpp"
#include "motor_task.hpp"
#include "robot_send_task.hpp"
#include "robot_task.hpp"

void TASK_CreateTask(void *pv);
[[noreturn]] void BSP_LoopTask(void *pv);

TaskHandle_t task_create_handle;

TaskHandle_t bsp_task_handle;
TaskHandle_t buzzer_task_handle;
TaskHandle_t debug_task_handle;
TaskHandle_t motor_task_handle;
TaskHandle_t robot_task_handle;
TaskHandle_t robot_send_task_handle;

void TASK_StartInit()
{
    BaseType_t xReturn;
    taskENTER_CRITICAL();
    xReturn = xTaskCreate((TaskFunction_t) TASK_CreateTask,
        "CreateTask",
        128,
        nullptr,
        2,
        &task_create_handle);
    taskEXIT_CRITICAL();
    if (pdFALSE != xReturn)
    {
        vTaskStartScheduler();
    }
}

void TASK_CreateTask(void *pv)
{
    BaseType_t xReturn = pdPASS;

    taskENTER_CRITICAL();
    xReturn &=
        xTaskCreate((TaskFunction_t) BSP_LoopTask,
        "BSPTask",
        128,
        nullptr,
        1,
        &bsp_task_handle);

    xReturn &=
        xTaskCreate((TaskFunction_t) BuzzerTask,
        "BuzzerTask",
        512,
        nullptr,
        3,
        &buzzer_task_handle);

    xReturn &=
        xTaskCreate((TaskFunction_t) DebugTask,
        "DebugTask",
        256,
        nullptr,
        2,
        &debug_task_handle);

    xReturn &=
        xTaskCreate((TaskFunction_t) MotorTask,
        "MotorTask",
        512,
        nullptr,
        6,
        &motor_task_handle);

    xReturn &=
        xTaskCreate((TaskFunction_t) RobotTask,
        "RobotTask",
        1024,
        nullptr,
        4,
        &robot_task_handle);

    xReturn &=
        xTaskCreate((TaskFunction_t) RobotSendTask,
        "RobotSendTask",
        1024,
        nullptr,
        4,
        &robot_send_task_handle);
    taskEXIT_CRITICAL();

    if (xReturn != pdPASS)
        Crash();
    vTaskDelete(task_create_handle);
}

void BSP_LoopTask(void *pv)
{
    while (true)
    {
        HAL_Delay(5);
        if (rc_data.IsRcOnline())
            HAL_IWDG_Refresh(&hiwdg1);
        // usart_printf("1\n");
    }
}
