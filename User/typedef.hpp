//
// Created by Glucose_carbide on 25-8-8
//

#pragma once

#include <cstdint>

#define OMNI    1
#define MECANUM 2
#define HELM    3

#define FSI6X   1
#define DT7     2

/// 遥控器拨杆
typedef enum
{
    HIGH = 1,
    MIDDLE = 3,
    LOW = 2
} eRemoteSwitchValue;

/// 底盘坐标系
typedef enum : uint8_t
{
    X = 0,
    Y = 1,
    Z = 2
} eChassisAxis;

/// 底盘状态
typedef enum : uint8_t
{
    NORMAL = 1,
    SPEED_UP = 2,
    EMERGENCY = 3
} eChassisState;

/// 电机转向
typedef enum : int8_t
{
    UP = 1,
    DOWN = -1
} eMotorDirection;

typedef enum
{
    PID_NORMAL = 0, //正常模式
    PID_RAMP = 1 //斜坡模式
} ePidMode;

/// 底盘CAN线发送与接收ID
typedef enum
{
    CAN_M3508_ALL_ID = 0x200,
    CAN_M3508_M1_ID = 0x201,
    CAN_M3508_M2_ID = 0x202,
    CAN_M3508_M3_ID = 0x203,
    CAN_M3508_M4_ID = 0x204,

    CAN_GM6020_ALL_ID = 0x1FF,
    CAN_GM6020_M1_ID = 0x205,
    CAN_GM6020_M2_ID = 0x206,
    CAN_GM6020_M3_ID = 0x207,
    CAN_GM6020_M4_ID = 0x208,

    CAN_RECEIVE_VEL_ID = 0x401, //接受云台的速度信息
    CAN_RECEIVE_AGL_ID = 0x402, //接受云台的角度信息

    CAN_SEND_SCAP_ID = 0x061,
    CAN_RECEIVE_SCAP_ID = 0x051,

    CAN_REFEREE_REC_ID = 0x405,
    CAN_SEND2SENTRY_ID = 0x406,
    CAN_REFEREE_SUB_ID = 0X407,
} eCan1MessageID;

/// 哈姆？哈姆。哈姆！
typedef enum
{
    CAN_DMJ4310_M1_RECEIVE_ID = 0x011,
    CAN_DMJ4310_M2_RECEIVE_ID = 0x012,
    CAN_DMJ4310_M3_RECEIVE_ID = 0x013,
    CAN_DMJ4310_M4_RECEIVE_ID = 0x014,

    CAN_DMJ4310_M1_SEND_ID = 0x001,
    CAN_DMJ4310_M2_SEND_ID = 0x002,
    CAN_DMJ4310_M3_SEND_ID = 0x003,
    CAN_DMJ4310_M4_SEND_ID = 0x004,
} eCan1MotorId_DM;

/// 哈姆的哈baby哈姆 哈姆的哈baby
typedef enum
{
    CAN_DMJ8009_M1_RECEIVE_ID = 0x011,
    CAN_DMJ8009_M2_RECEIVE_ID = 0x012,
    CAN_DMJ8009_M3_RECEIVE_ID = 0x013,
    CAN_DMJ8009_M4_RECEIVE_ID = 0x014,

    CAN_DMJ8009_M1_SEND_ID = 0x001,
    CAN_DMJ8009_M2_SEND_ID = 0x002,
    CAN_DMJ8009_M3_SEND_ID = 0x003,
    CAN_DMJ8009_M4_SEND_ID = 0x004,
} eCan2MotorId_DM;
