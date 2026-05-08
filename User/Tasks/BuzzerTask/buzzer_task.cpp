//
// Created by Glucose_carbide on 2026-05-02.
//

#include "buzzer_task.hpp"

#include <iterator>

#include "config.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "buzzer.hpp"
#include "motor_sts.hpp"
#include "music_soundtrack.hpp"
#include "user_soundtrack.h"
#include "remote.hpp"

void BuzzerTask(void *pv)
{
    bool online = false;
    const Buzzer::Config motor_timeout = {
        Buzzer::Soundtracks::timeout, std::size(Buzzer::Soundtracks::timeout),
        10, &online, false, true
    };
    Buzzer::AddConfig(motor_timeout);
    portTickType current_time = xTaskGetTickCount();         //获取当前tick（时间）
    while (true)
    {
        Buzzer::ControlLoop();
        online = MotorSts::IsConnected();
        vTaskDelayUntil(&current_time, pdMS_TO_TICKS(TASK_BUZZER_TASK_PERIOD));
    }
}