//
// Created by Glucose_carbide on 2026-05-02.
//

#include "buzzer_task.hpp"

#include <iterator>

#include "config.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "buzzer.hpp"
#include "music_soundtrack.hpp"
#include "user_soundtrack.h"
#include "remote.hpp"

void BuzzerTask(void *pv)
{
    bool sa = false;
    bool sb = false;
    bool sd = false;
    const Buzzer::Config cfg_sa = {
        Buzzer::Soundtracks::idoratrize_world, std::size(Buzzer::Soundtracks::idoratrize_world),
        10, &sa, true, true
    };
    const Buzzer::Config cfg_sb = {
        Buzzer::Soundtracks::immortal_ash, std::size(Buzzer::Soundtracks::immortal_ash),
        9, &sb, true, true
    };
    const Buzzer::Config cfg_sc = {
        Buzzer::Soundtracks::watery_graves, std::size(Buzzer::Soundtracks::watery_graves),
        8, &sd, true, true
    };
    // Buzzer::AddConfig(cfg_sa);
    // Buzzer::AddConfig(cfg_sb);
    // Buzzer::AddConfig(cfg_sc);
    portTickType current_time = xTaskGetTickCount();         //获取当前tick（时间）
    while (true)
    {
        Buzzer::ControlLoop();
        sa = rc_data.GetRcSwitchA() != eRemoteSwitchValue::HIGH;
        sb = rc_data.GetRcSwitchB() != eRemoteSwitchValue::HIGH;
        sd = rc_data.GetRcSwitchD() != eRemoteSwitchValue::HIGH;
        vTaskDelayUntil(&current_time, pdMS_TO_TICKS(TASK_BUZZER_TASK_PERIOD));
    }
}