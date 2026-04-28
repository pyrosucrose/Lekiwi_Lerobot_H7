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
#include "iwdg.h"
#include "remote.hpp"
#include "robot_send_task.hpp"
#include "robot_task.hpp"
#include "usartio.hpp"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ==================================
void TASK_CreateTask(void *pv);
void BSP_LoopTask(void *pv);

// =============================== 函数实现 ===============================

TaskHandle_t task_create_handle;

TaskHandle_t robot_task_handle;
TaskHandle_t robot_send_task_handle;
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
    BaseType_t xReturn = pdPASS;

    taskENTER_CRITICAL();
    xReturn &=
        xTaskCreate((TaskFunction_t) BuzzerTask,
        "BuzzerTask",
        512,
        nullptr,
        3,
        &buzzer_task_handle);

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

    xReturn &=
        xTaskCreate((TaskFunction_t) DebugTask,
        "DebugTask",
        128,
        nullptr,
        1,
        &debug_task_handle);

    xReturn &=
        xTaskCreate((TaskFunction_t) BSP_LoopTask,
        "BSPTask",
        128,
        (void *) nullptr,
        1,
        &bsp_task_handle);
    taskEXIT_CRITICAL();

    if (xReturn != pdPASS)
        Error_Handler();
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
