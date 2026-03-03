//
// Created by Glucose_carbide on 25-8-8.
//

// =============================== 引入头文件 ===============================
#include "start_task.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "buzzer_task.hpp"
#include "debug_task.hpp"
#include "usart.h"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ==================================
void TASK_CreateTask(void *pv);
void BSP_LoopTask(void *pv);

// =============================== 函数实现 ===============================

TaskHandle_t task_create_handle;

TaskHandle_t buzzer_task_handle;
TaskHandle_t debug_task_handle;
TaskHandle_t bsp_task_handle;

void TASK_StartInit()
{
    BaseType_t xReturn;
    taskENTER_CRITICAL();
    xReturn = xTaskCreate((TaskFunction_t) TASK_CreateTask,
        "CreateTask",
        128,
        (void *) nullptr,
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
    BaseType_t xReturn;
    taskENTER_CRITICAL();

    xTaskCreate((TaskFunction_t) BuzzerTask,
        "BuzzerTask",
        128,
        nullptr,
        4,
        &buzzer_task_handle);

    xTaskCreate((TaskFunction_t) DebugTask,
        "DebugTask",
        128,
        nullptr,
        1,
        &debug_task_handle);

    //
    // xTaskCreate((TaskFunction_t) RefereeTask,
    //     "RefereeTask",
    //     512,
    //     (void *) NULL,
    //     6,
    //     &referee_task_handle);

    xReturn = xTaskCreate((TaskFunction_t) BSP_LoopTask,
        "BSPTask",
        128,
        (void *) nullptr,
        1,
        &bsp_task_handle);

    taskEXIT_CRITICAL();
    if (pdFALSE != xReturn)
        vTaskDelete(task_create_handle);
}

void BSP_LoopTask(void *pv)
{
    while (true)
    {
        vTaskDelay(5);
    }
}

// int16_t cmd_control = 0;
