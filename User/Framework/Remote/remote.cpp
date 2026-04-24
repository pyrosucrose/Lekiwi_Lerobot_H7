//
// Created by Glucose_carbide on 25-8-8.
//

// =============================== 引入头文件 ===============================
#include "remote.hpp"

// =============================== 宏定义区 ===============================
#if REMOTE_TYPE == DT7
static constexpr uint16_t RC_OFFSET         = 1024;
static constexpr uint16_t RC_FRAME_LENGTH   = 18u;
static constexpr float  RC_CHANNEL_MAX      = 660.0f;
#elif REMOTE_TYPE == FSI6X
static constexpr uint16_t RC_OFFSET         = 1024;
static constexpr uint16_t RC_FRAME_LENGTH   = 25u;
static constexpr float RC_CHANNEL_MAX       = 784.0f;
#define FSI6X_CHANNEL_1                 rc_right_horizontal
#define FSI6X_CHANNEL_2                 rc_right_vertical
#define FSI6X_CHANNEL_3                 rc_left_vertical
#define FSI6X_CHANNEL_4                 rc_left_horizontal
#define FSI6X_CHANNEL_5                 rc_knob_left
#define FSI6X_CHANNEL_6                 rc_knob_right
#define FSI6X_CHANNEL_7                 rc_switch_A
#define FSI6X_CHANNEL_8                 rc_switch_B
#define FSI6X_CHANNEL_9                 rc_switch_C
#define FSI6X_CHANNEL_10                rc_switch_D
#endif

static constexpr float RC_CHANNEL_RAW_2_TRANSLATION = CHASSIS_TRANSLATION_SPEED_MAX / RC_CHANNEL_MAX;
static constexpr float RC_CHANNEL_RAW_2_ROTATION    = CHASSIS_ROTATION_SPEED_MAX / RC_CHANNEL_MAX;

// =============================== 变量区 ==================================
cRemoteData rc_data;

// =============================== 函数实现 ===============================

static int16_t ReturnSwitchStatus(const int16_t status)
{
    if (status < 0)
        return HIGH;
    if (status == 0)
        return MIDDLE;
    if (status > 0)
        return LOW;
    switch (status)
    {
    case -784:
        return HIGH; // 1(这是一个枚举，对应的值就是1，下同)
    case 0:
        return MIDDLE; // 3
    case 783:
        return LOW; // 2
    default:
        return HIGH; // 1
    }
}

void cRemoteData::RcUpdateCounter()
{
// #ifdef CONFIG_REMOTE_TYPE_DT7
    counter++;
    if (counter >= 200)
    {
        counter = 1024;
        rc_online = false;
    }
// #endif // CONFIG_REMOTE_TYPE_DT7
}

void cRemoteData::RcUpdateValue(const uint8_t *rc_data)
{
#if REMOTE_TYPE == DT7
    if (rc_data == nullptr)
        return;

    rc_right_horizontal = static_cast<int16_t>(((rc_data[1] <<  8 | rc_data[0])                        & 0x7FF) - RC_OFFSET);
    rc_right_vertical   = static_cast<int16_t>(((rc_data[2] <<  5 | rc_data[1] >> 3)                   & 0x7FF) - RC_OFFSET);
    rc_left_horizontal  = static_cast<int16_t>(((rc_data[4] << 10 | rc_data[3] << 2 | rc_data[2] >> 6) & 0x7FF) - RC_OFFSET);
    rc_left_vertical    = static_cast<int16_t>(((rc_data[5] <<  7 | rc_data[4] >> 1)                   & 0x7FF) - RC_OFFSET);
    rc_switch_A = static_cast<int16_t>((rc_data[5] >> 4 & 0x0C) >> 2);
    rc_switch_D = static_cast<int16_t>( rc_data[5] >> 4 & 0x03);

    rc_left_horizontal_float    = static_cast<float>(rc_left_horizontal)  * RC_CHANNEL_RAW_2_TRANSLATION;
    rc_left_vertical_float      = static_cast<float>(rc_left_vertical)    * RC_CHANNEL_RAW_2_TRANSLATION;
    rc_right_horizontal_float   = static_cast<float>(rc_right_horizontal) * RC_CHANNEL_RAW_2_ROTATION;

    mouse_x = static_cast<int16_t>(rc_data[7] << 8 | rc_data[6]);
    mouse_y = static_cast<int16_t>(rc_data[9] << 8 | rc_data[8]);
    mouse_z = static_cast<int16_t>(rc_data[11] << 8 | rc_data[10]);
    mouse_pressed_left = rc_data[12];
    mouse_pressed_right = rc_data[13];

    keyboard_values = rc_data[15] << 8 | rc_data[14];
    pressed_W		= static_cast<uint8_t>(keyboard_values & 0x01);
    pressed_S		= static_cast<uint8_t>(keyboard_values & 0x02);
    pressed_A		= static_cast<uint8_t>(keyboard_values & 0x04);
    pressed_D		= static_cast<uint8_t>(keyboard_values & 0x08);
    pressed_Q		= static_cast<uint8_t>(keyboard_values & 0x10);
    pressed_E		= static_cast<uint8_t>(keyboard_values & 0x20);
    pressed_Shift	= static_cast<uint8_t>(keyboard_values & 0x40);
    pressed_Ctrl	= static_cast<uint8_t>(keyboard_values & 0x80);

    counter = 0; rc_online = true;
#elif REMOTE_TYPE == FSI6X
    if (rc_data[0] != 0x0F || rc_data[24] != 0x00)
    {
        rc_online = false;
        return;
    }

    FSI6X_CHANNEL_1  = static_cast<int16_t>(((rc_data[2 ] <<  8 | rc_data[1 ])                        & 0x7FF) - RC_OFFSET);
    FSI6X_CHANNEL_2  = static_cast<int16_t>(((rc_data[3 ] <<  5 | rc_data[2 ] >> 3)                   & 0x7FF) - RC_OFFSET);
    FSI6X_CHANNEL_3  = static_cast<int16_t>(((rc_data[5 ] << 10 | rc_data[4 ] << 2 | rc_data[3] >> 6) & 0x7FF) - RC_OFFSET);
    FSI6X_CHANNEL_4  = static_cast<int16_t>(((rc_data[6 ] <<  7 | rc_data[5 ] >> 1)                   & 0x7FF) - RC_OFFSET);
    FSI6X_CHANNEL_5  = static_cast<int16_t>(((rc_data[7 ] <<  4 | rc_data[6 ] >> 4)                   & 0x7FF) - RC_OFFSET);
    FSI6X_CHANNEL_6  = static_cast<int16_t>(((rc_data[9 ] <<  9 | rc_data[8 ] << 1 | rc_data[7] >> 7) & 0x7FF) - RC_OFFSET);
    FSI6X_CHANNEL_7  = static_cast<int16_t>(((rc_data[10] <<  6 | rc_data[9 ] >> 2)                   & 0x7FF) - RC_OFFSET);
    FSI6X_CHANNEL_8  = static_cast<int16_t>(((rc_data[11] <<  3 | rc_data[10] >> 5)                   & 0x7FF) - RC_OFFSET);
    FSI6X_CHANNEL_9  = static_cast<int16_t>(((rc_data[13] <<  8 | rc_data[12])                        & 0x7FF) - RC_OFFSET);
    FSI6X_CHANNEL_10 = static_cast<int16_t>(((rc_data[14] <<  5 | rc_data[13] >> 3)                   & 0x7FF) - RC_OFFSET);

    rc_left_horizontal_float    = static_cast<float>(rc_left_horizontal)  / RC_CHANNEL_MAX;
    rc_left_vertical_float      = static_cast<float>(rc_left_vertical)    / RC_CHANNEL_MAX;
    rc_right_horizontal_float   = static_cast<float>(rc_right_horizontal) / RC_CHANNEL_MAX;
    rc_right_vertical_float     = static_cast<float>(rc_right_vertical)   / RC_CHANNEL_MAX;

    // usart_printf("%d,%d,%d,%d\n",rc_left_horizontal, rc_left_vertical, rc_right_horizontal, rc_right_vertical);
    // usart_printf("%.3f,%.3f,%.3f,%.3f\n",rc_left_horizontal_float, rc_left_vertical_float, rc_right_horizontal_float, rc_right_vertical_float);

    rc_switch_A = ReturnSwitchStatus(rc_switch_A);
    rc_switch_B = ReturnSwitchStatus(rc_switch_B);
    rc_switch_C = ReturnSwitchStatus(rc_switch_C);
    rc_switch_D = ReturnSwitchStatus(rc_switch_D);

    if (rc_data[23] >> 2 & 0x01 || rc_data[23] >> 3 & 0x01)
    {
        counter = 1145;
        rc_online = false;
    }
    else
    {
        rc_online = true;
        counter = 0;
    }

    rc_left_horizontal_last = rc_left_horizontal;
    rc_left_vertical_last = rc_left_vertical;
    rc_right_horizontal_last = rc_right_horizontal;
    rc_right_vertical_last = rc_right_vertical;
    rc_switch_C_last = rc_switch_C;
#endif // CONFIG_REMOTE_TYPE_DT7
}
