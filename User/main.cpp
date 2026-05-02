//
// Created by Glucose_carbide on 2026-05-02.
//

#include "main.h"
#include "usartio.hpp"
#include "start_task.hpp"
#include "buzzer.hpp"
#include "delay.hpp"
#include "motor_sts.hpp"

void BSP_Init();
void User_Init()
{
    Usartio_Init();
    Buzzer::Init();
    Delay::Init();
    MotorSts::Init();
    TASK_StartInit();
}

[[noreturn]] int main()
{
    BSP_Init();
    Buzzer::SetTone(0);
    auto s = Delay::GetTimeStamp();
    while (true)
    {
        // Delay::Delay_ms(1000);
        usart_printf("1\n");
        Delay::DelayUntil_us(&s,50000);
        // for (uint32_t i = 0; i < 1000000; i++){}
        // usart_printf("%f\n",Delay::CalculateInterval_us(s));
    }
}

void BSP_Init()
{
    MX_Main_Init();
    User_Init();
}
