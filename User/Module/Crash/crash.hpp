//
// Created by Glucose_carbide on 2026-04-29.
//

#pragma once

#include "buzzer.hpp"
#include "main.h"

/**
 * @brief   崩溃处理，将蜂鸣器频率置为2000并进入err_handler
 * @note    若是初始化时故障，表现则为卡死；若是运行时故障，则会鸣叫蜂鸣器
 * @note    用户可通过调试模式查看程序从哪进了这，一般调用处都会配备instruction(我应该是写了的吧...)
 * @remark  tbh有点意义不明，其实只是换皮err_handler
 */
inline void Crash()
{
    Buzzer::SetTone(2000);
    Error_Handler();
}
