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
    // MotorSts motor(1,2048,1000,3000,false);
    while (true)
    {
        // auto s = Delay::GetTimeStamp();
        robot_lekiwi.SendControl();
        // usart_printf("%f,%f,%f,%f,%f,%f\n",
        //     MotorSts::motors_[MotorSts::motors_idx_[11]]->GetSoftPos_One(),
        //     MotorSts::motors_[MotorSts::motors_idx_[12]]->GetSoftPos_One(),
        //     MotorSts::motors_[MotorSts::motors_idx_[13]]->GetSoftPos_One(),
        //     MotorSts::motors_[MotorSts::motors_idx_[14]]->GetSoftPos_One(),
        //     MotorSts::motors_[MotorSts::motors_idx_[15]]->GetSoftPos_One(),
        //     MotorSts::motors_[MotorSts::motors_idx_[16]]->GetSoftPos_One()
        // );

        // MotorSts::ReadAll(MotorSts::REG::NOW_POS_L, MotorSts::REG::NOW_TEMP);
        // usart_printf("%f\n",Delay::CalculateInterval_us(s));
        // usart_printf("%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
        //     MotorSts::motors_[0]->pos_ecd_,
        //     MotorSts::motors_[1]->pos_ecd_,
        //     MotorSts::motors_[2]->pos_ecd_,
        //     MotorSts::motors_[3]->pos_ecd_,
        //     MotorSts::motors_[4]->pos_ecd_,
        //     MotorSts::motors_[5]->pos_ecd_,
        //     MotorSts::motors_[6]->pos_ecd_,
        //     MotorSts::motors_[7]->pos_ecd_,
        //     MotorSts::motors_[8]->pos_ecd_
        //     );

        // usart_printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
        //     MotorSts::motors_[0]->GetSoftPos_One(),
        //     MotorSts::motors_[1]->GetSoftPos_One(),
        //     MotorSts::motors_[2]->GetSoftPos_One(),
        //     MotorSts::motors_[3]->GetSoftPos_One(),
        //     MotorSts::motors_[4]->GetSoftPos_One(),
        //     MotorSts::motors_[5]->GetSoftPos_One(),
        //     MotorSts::motors_[6]->GetSoftPos_One(),
        //     MotorSts::motors_[7]->GetSoftPos_One(),
        //     MotorSts::motors_[8]->GetSoftPos_One()
        //     );

        // usart_printf("%d,%d,%d,%d,%d\n",
        //     MotorSts::motors_[0]->soft_pos_ecd,
        //     MotorSts::motors_[0]->soft_min_pos_ecd_,
        //     MotorSts::motors_[0]->soft_max_pos_ecd_,
        //     MotorSts::motors_[0]->zero_point_ecd_,
        //     MotorSts::motors_[0]->pos_ecd_
        //     );

        // usart_printf("%d,%d,%d,%d,%d,%d\n",
        //     robot_lekiwi.arm.motors[0].pos_ecd_,
        //     robot_lekiwi.arm.motors[1].pos_ecd_,
        //     robot_lekiwi.arm.motors[2].pos_ecd_,
        //     robot_lekiwi.arm.motors[3].pos_ecd_,
        //     robot_lekiwi.arm.motors[4].pos_ecd_,
        //     robot_lekiwi.arm.motors[5].pos_ecd_
        //     );

        // usart_printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
        //     robot_lekiwi.arm.motors[1].soft_min_pos_ecd_,
        //     robot_lekiwi.arm.motors[1].soft_max_pos_ecd_,
        //     robot_lekiwi.arm.motors[2].soft_min_pos_ecd_,
        //     robot_lekiwi.arm.motors[2].soft_max_pos_ecd_,
        //     robot_lekiwi.arm.motors[3].soft_min_pos_ecd_,
        //     robot_lekiwi.arm.motors[3].soft_max_pos_ecd_,
        //     robot_lekiwi.arm.motors[4].soft_min_pos_ecd_,
        //     robot_lekiwi.arm.motors[4].soft_max_pos_ecd_,
        //     robot_lekiwi.arm.motors[5].soft_min_pos_ecd_,
        //     robot_lekiwi.arm.motors[5].soft_max_pos_ecd_
        //     );

        // static uint16_t dum = 0; dum++;
        // if (dum >= 100)
        // {
        //     static uint16_t a1 = 0,a2 = 0,a3 = 0,a4 = 0,a5 = 0,a6 = 0;
        //     static uint16_t b1 = 11451,b2 = 11451,b3 = 11451,b4 = 11451,b5 = 11451,b6 = 11451;
        //     a1 = std::max(a1, MotorSts::motors_[MotorSts::motors_idx_[11]]->GetHardPos_Raw());
        //     a2 = std::max(a2, MotorSts::motors_[MotorSts::motors_idx_[12]]->GetHardPos_Raw());
        //     a3 = std::max(a3, MotorSts::motors_[MotorSts::motors_idx_[13]]->GetHardPos_Raw());
        //     a4 = std::max(a4, MotorSts::motors_[MotorSts::motors_idx_[14]]->GetHardPos_Raw());
        //     a5 = std::max(a5, MotorSts::motors_[MotorSts::motors_idx_[15]]->GetHardPos_Raw());
        //     a6 = std::max(a6, MotorSts::motors_[MotorSts::motors_idx_[16]]->GetHardPos_Raw());
        //
        //     b1 = std::min(b1, MotorSts::motors_[MotorSts::motors_idx_[11]]->GetHardPos_Raw());
        //     b2 = std::min(b2, MotorSts::motors_[MotorSts::motors_idx_[12]]->GetHardPos_Raw());
        //     b3 = std::min(b3, MotorSts::motors_[MotorSts::motors_idx_[13]]->GetHardPos_Raw());
        //     b4 = std::min(b4, MotorSts::motors_[MotorSts::motors_idx_[14]]->GetHardPos_Raw());
        //     b5 = std::min(b5, MotorSts::motors_[MotorSts::motors_idx_[15]]->GetHardPos_Raw());
        //     b6 = std::min(b6, MotorSts::motors_[MotorSts::motors_idx_[16]]->GetHardPos_Raw());
        //
        //     usart_printf("%d,%d,%d,%d,%d,%d; %d,%d,%d,%d,%d,%d\n",
        //         a1,a2,a3,a4,a5,a6,b1,b2,b3,b4,b5,b6);
        // }

        vTaskDelayUntil(&current_time, pdMS_TO_TICKS(TASK_ROBOT_SEND_TASK_PERIOD));
    }
}