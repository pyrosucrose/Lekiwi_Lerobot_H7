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
    if constexpr (!MotorSts::USE_MOTOR_POS_LIMIT)
        MotorSts::Init();
    TASK_StartInit();
}

[[noreturn]] int main()
{
    BSP_Init();
    Buzzer::SetTone(0);
    auto s = Delay::GetTimeStamp();
    MotorSts motor1(11,2035,720,3440,false);
    MotorSts motor2(12,1906,830,3200,false);
    MotorSts motor3(13,2457,883,3080,false);
    MotorSts motor4(14,1671,862,2900,false);
    MotorSts motor5(15,1503,300,3980,false);
    MotorSts motor6(16,1702,1400,2870,false);
    while (true)
    {
        HAL_Delay(10);
        // motor1.TransmitWriteCommand(MotorSts::REG::LOCK, 0); HAL_Delay(1); // motor1.TransmitWriteCommand(MotorSts::REG::ID, 11); HAL_Delay(1);
        // motor2.TransmitWriteCommand(MotorSts::REG::LOCK, 0); HAL_Delay(1); // motor2.TransmitWriteCommand(MotorSts::REG::ID, 12); HAL_Delay(1);
        // motor3.TransmitWriteCommand(MotorSts::REG::LOCK, 0); HAL_Delay(1); // motor3.TransmitWriteCommand(MotorSts::REG::ID, 13); HAL_Delay(1);
        // motor4.TransmitWriteCommand(MotorSts::REG::LOCK, 0); HAL_Delay(1); // motor4.TransmitWriteCommand(MotorSts::REG::ID, 14); HAL_Delay(1);
        // motor5.TransmitWriteCommand(MotorSts::REG::LOCK, 0); HAL_Delay(1); // motor5.TransmitWriteCommand(MotorSts::REG::ID, 15); HAL_Delay(1);
        // motor6.TransmitWriteCommand(MotorSts::REG::LOCK, 0); HAL_Delay(1); // motor6.TransmitWriteCommand(MotorSts::REG::ID, 16); HAL_Delay(1);
        MotorSts::ReadAll(MotorSts::REG::NOW_POS_L, MotorSts::REG::NOW_TEMP);
        HAL_Delay(10);
        // motor1.AddReadRange(MotorSts::REG::NOW_POS_L, MotorSts::REG::NOW_TEMP); motor1.TransmitReadCommand();
        usart_printf("%f,%f,%f,%f,%f,%f\n",
            motor1.GetSoftPos_One(),
            motor2.GetSoftPos_One(),
            motor3.GetSoftPos_One(),
            motor4.GetSoftPos_One(),
            motor5.GetSoftPos_One(),
            motor6.GetSoftPos_One()
            );
        MotorSts::UpdateAll();
        // Delay::Delay_ms(1000);
        // usart_printf("1\n");
        // Delay::DelayUntil_us(&s,50000);
        // for (uint32_t i = 0; i < 1000000; i++){}
        // usart_printf("%f\n",Delay::CalculateInterval_us(s));
    }
}

void BSP_Init()
{
    MX_Main_Init();
    User_Init();
}
