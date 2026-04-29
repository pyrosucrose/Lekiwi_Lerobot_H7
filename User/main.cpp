//
// Created by Glucose_carbide on 25-7-23.
//

#include "main.h"
#include "usartio.hpp"
#include "start_task.hpp"
#include "buzzer.hpp"
#include "delay.hpp"

void BSP_Init();
void User_Init()
{
    Usartio_Init();
    Buzzer::Init();
    Delay::Init();
    TASK_StartInit();
}

[[noreturn]] int main()
{
    BSP_Init();
    while (true);
}

void BSP_Init()
{
    MX_Main_Init();
    User_Init();
}
