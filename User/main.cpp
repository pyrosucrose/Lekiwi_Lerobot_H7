//
// Created by Glucose_carbide on 25-7-23.
//

#include "main.h"
#include "cmsis_os.h"
// #include "iwdg.h"
#include "usartio.hpp"
#include "start_task.hpp"
#include "motor_sts.hpp"
#include "config.hpp"
#include "buzzer.h"
#include "remote.hpp"
#include "robot.hpp"

void BSP_Init();
void User_Init()
{
    Usartio_Init();
    Buzzer::Init();
    TASK_StartInit();
}

int main()
{
    BSP_Init();
    // HAL_IWDG_Refresh(&hiwdg);
    // cBuzzer buzzer(&htim12, TIM_CHANNEL_2);
    // buzzer.Init();
    // buzzer.ChangeFreqDuty(re_4); HAL_Delay(500);
    // buzzer.ChangeFreqDuty(mi_4); HAL_Delay(500);
    // buzzer.ChangeFreqDuty(fa_4); HAL_Delay(500);
    // buzzer.ChangeFreqDuty(so_4); HAL_Delay(500);
    // buzzer.ChangeFreqDuty(la_4); HAL_Delay(500);
    // buzzer.ChangeFreqDuty(si_4); HAL_Delay(500);
    // buzzer.ChangeFreqDuty(do_5); HAL_Delay(500);
    // buzzer.ChangeFreqDuty(re_5); HAL_Delay(500);

    while (true)
    {
        // usart_printf("1\n");
        // robot_lekiwi.ControlLoop();
        // buzzer.ControlLoop();
        HAL_Delay(TASK_BUZZER_TASK_PERIOD);
    }
    // while (true)
    // {
    //     for (auto& motor : motors)
    //     {
    //         // motor.AddReadReg(cMotorSts::REG::PHASE);
    //         // motor.TransmitWriteCommand(cMotorSts::REG::PHASE, 0x0C);
    //         motor.AddReadRange(cMotorSts::REG::NOW_POS_L, cMotorSts::REG::NOW_TEMP);
    //         motor.TransmitReadCommand();
    //         HAL_Delay(2);
    //     }
    //
    //     // for (int i = 1; i <= 6; i++)
    //     // {
    //     //     uint8_t kp = 0, ki = 0;
    //     //     switch (i)
    //     //     {
    //     //     case 1: kp = 0x40; ki = 0x10; break;
    //     //     case 2: kp = 0x40; ki = 0x08; break;
    //     //     case 3: kp = 0x40; ki = 0x08; break;
    //     //     case 4: kp = 0x30; ki = 0x08; break;
    //     //     case 5: kp = 0x30; ki = 0x08; break;
    //     //     case 6: kp = 0x30; ki = 0x08; break;
    //     //     default: break;
    //     //     }
    //     //     // motors[i].AddReadReg(cMotorSts::REG::POS_P_GAIN);
    //     //     // motors[i].TransmitReadCommand();
    //     //     // HAL_Delay(2);
    //     //     // motors[i].AddReadReg(cMotorSts::REG::POS_I_GAIN);
    //     //     // motors[i].TransmitReadCommand();
    //     //     // HAL_Delay(2);
    //     //     // motors[i].TransmitWriteCommand(cMotorSts::REG::POS_P_GAIN, kp);
    //     //     // HAL_Delay(2);
    //     //     // motors[i].TransmitWriteCommand(cMotorSts::REG::POS_I_GAIN, ki);
    //     //     // HAL_Delay(2);
    //     // }
    //
    //     // static int16_t pos = 2048; static bool direction = true;
    //     // if (pos >= 2300) direction = false;
    //     // if (pos <= 1700) direction = true;
    //     // if (direction)
    //     //     pos += 8;
    //     // else
    //     //     pos -= 8;
    //     // for (int i = 0; i < 9; i++)
    //     // {
    //     //
    //     //     if (i < 6)
    //     //     {
    //     //         // motors[i].TransmitWriteCommand(cMotorSts::REG::TARGET_POSITION_L, pos);
    //     //         HAL_Delay(2);
    //     //     }
    //     //     // else
    //     //     // {
    //     //     //     static int16_t speed = 0; static bool direction = true;
    //     //     //     if (speed >= 1000) direction = false;
    //     //     //     if (speed <= -1000) direction = true;
    //     //     //     if (direction)
    //     //     //         speed += 5;
    //     //     //     else
    //     //     //         speed -= 5;
    //     //     //     motors[i].TransmitWriteCommand(cMotorSts::REG::TARGET_SPEED_L, speed);
    //     //     //     HAL_Delay(2);
    //     //     // }
    //     // }
    //
    //     // static float tx = 170.0f,ty = 170.0f,tw = 0.0f;
    //     // static bool direction = true;
    //     // static int cnt = 0;
    //     // cnt++; cnt %= 700;
    //     // if (cnt >= 0 && cnt <= 300)
    //     // {
    //     //     if (ty >= 250.0f) direction = false;
    //     //     if (ty <= 100.0f) direction = true;
    //     //     if (direction)
    //     //         ty += 1.0f;
    //     //     else
    //     //         ty -= 1.0f;
    //     // }
    //     //
    //     // if (cnt >= 300 && cnt <= 500)
    //     // {
    //     //     if (tx >= 250.0f) direction = false;
    //     //     if (tx <= 150.0f) direction = true;
    //     //     if (direction)
    //     //         tx += 1.0f;
    //     //     else
    //     //         tx -= 1.0f;
    //     // }
    //     //
    //     // if (cnt >= 500 && cnt <= 700)
    //     // {
    //     //     if (tw >= 0.7f) direction = false;
    //     //     if (tw <= -0.3f) direction = true;
    //     //     if (direction)
    //     //         tw += 0.01f;
    //     //     else
    //     //         tw -= 0.01f;
    //     // }
    //     static float tx = 110.0f, ty = -110.0f, tw = -1.0f;
    //     tx += rc_data.GetRcRightHorizontal();
    //     ty += rc_data.GetRcRightVertical();
    //     // tw = rc_data.GetRcKnobLeft() * 1.7f / 784.0f;
    //
    //     // usart_printf("%f,%f,%f\n",tx,ty,tw);
    //     arm.SetTarget(tx, ty, tw);
    //     // arm.SetTarget(110,-110,-1);
    //     if (arm.SolveArm())
    //     {
    //         // usart_printf("%d,%d,%d\n",arm.target_angle_ecd[1],arm.target_angle_ecd[2],arm.target_angle_ecd[3]);
    //         // usart_printf("%d,%d,%d\n",arm.alpha,arm.beta,arm.gamma);
    //         motors[1].TransmitWriteCommand(cMotorSts::REG::TARGET_POSITION_L, static_cast<int16_t>(arm.a));
    //         HAL_Delay(2);
    //         motors[2].TransmitWriteCommand(cMotorSts::REG::TARGET_POSITION_L, static_cast<int16_t>(arm.b));
    //         HAL_Delay(2);
    //         motors[3].TransmitWriteCommand(cMotorSts::REG::TARGET_POSITION_L, static_cast<int16_t>(arm.c));
    //         HAL_Delay(2);
    //         // arm.DeSolveArm(motors[1].pos_ecd, motors[2].pos_ecd, motors[3].pos_ecd);
    //     }
    //     else
    //     {
    //         // usart_printf("F\n");
    //     }
    //     // arm.DeSolveArm(static_cast<int16_t>(arm.a), static_cast<int16_t>(arm.b), static_cast<int16_t>(arm.c));
    //     HAL_Delay(2);
    //
    //     // usart_printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",pos,
    //     // motors[0].pos_ecd, motors[1].pos_ecd, motors[2].pos_ecd,
    //     // motors[3].pos_ecd, motors[4].pos_ecd, motors[5].pos_ecd,
    //     // motors[6].vel_ecd, motors[7].vel_ecd, motors[8].vel_ecd
    //     // );
    // }
}

void BSP_Init()
{
    MX_Main_Init();
    User_Init();
}
