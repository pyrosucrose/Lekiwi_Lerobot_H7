//
// Created by Glucose_carbide on 25-8-8.
//

#pragma once

// =============================== 调用库 ===============================
#include "motor_sts.hpp"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ===============================

// =============================== 函数声明 ===============================

// =============================== 类声明 ===============================

class cChassis
{
    friend class cRobot;
public:
    cChassis():
        motors
        {
        {8,0,0,0,false},
        {9,0,0,0,false},
        {7,0,0,0,false},
        }
    {}


    void ControlLoop();
    void TransmitBusControlCmd();
    void DisableAll();

    void GetDataFromRc();
    void SolveWheelSpeed();

    float target_speed[3] = {};
    float target_wheel_speed[3] = {};
    cMotorSts motors[3];
};
