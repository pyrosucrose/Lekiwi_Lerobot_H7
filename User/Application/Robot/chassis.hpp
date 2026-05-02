//
// Created by Glucose_carbide on 2026-05-02.
//

#pragma once

#include "motor_sts.hpp"

class LekiwiChassis
{
    friend class RobotLekiwi;
public:
    LekiwiChassis():
        motors
        {
        {8,0,0,0,true},
        {9,0,0,0,true},
        {7,0,0,0,true},
        }
    {}


    void ControlLoop();
    void TransmitBusControlCmd();
    void DisableAll();

    void GetDataFromRc();
    void SolveWheelSpeed();

    float target_speed[3] = {};
    float target_wheel_speed[3] = {};
    MotorSts motors[3];
};
