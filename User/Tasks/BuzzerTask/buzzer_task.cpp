//
// Created by Glucose_carbide on 25-8-8.
//

// =============================== 引入头文件 ===============================
#include "buzzer_task.hpp"

#include <iterator>

#include "config.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "buzzer.hpp"
#include "delay.hpp"
#include "music_soundtrack.hpp"
#include "remote.hpp"
#include "usartio.hpp"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ==================================

// =============================== 函数实现 ===============================

void BuzzerTask(void *pv)
{
    bool sa = false;
    bool sb = false;
    bool sd = false;
    const Buzzer::Config cfg_sa = {
        Buzzer::Soundtracks::soundtrack_idoratrize_world, std::size(Buzzer::Soundtracks::soundtrack_idoratrize_world),
        10, &sa, true, true
    };
    const Buzzer::Config cfg_sb = {
        Buzzer::Soundtracks::soundtrack_immortal_ash, std::size(Buzzer::Soundtracks::soundtrack_immortal_ash),
        9, &sb, true, true
    };
    const Buzzer::Config cfg_sc = {
        Buzzer::Soundtracks::soundtrack_watery_graves, std::size(Buzzer::Soundtracks::soundtrack_watery_graves),
        8, &sd, true, true
    };
    // Buzzer::AddConfig(cfg_sa);
    // Buzzer::AddConfig(cfg_sb);
    // Buzzer::AddConfig(cfg_sc);
    portTickType current_time = xTaskGetTickCount();         //获取当前tick（时间）
    while (true)
    {
        // usart_printf("2\n");
        // auto s = Delay::GetTimeStamp();
        Buzzer::ControlLoop();
        sa = rc_data.GetRcSwitchA() != eRemoteSwitchValue::HIGH;
        sb = rc_data.GetRcSwitchB() != eRemoteSwitchValue::HIGH;
        sd = rc_data.GetRcSwitchD() != eRemoteSwitchValue::HIGH;
        vTaskDelayUntil(&current_time, pdMS_TO_TICKS(TASK_BUZZER_TASK_PERIOD));
        // usart_printf("%f\n",Delay::CalculateInterval_us(s));
    }
}