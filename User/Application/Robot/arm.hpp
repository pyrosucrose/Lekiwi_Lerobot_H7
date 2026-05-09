//
// Created by Glucose_carbide on 2026-05-02.
//

#pragma once

#include "motor_sts.hpp"

class LekiwiArm
{
    friend class RobotLekiwi;
public:
    LekiwiArm() :
        motors
        {
        {1,2035,720 ,3440,false,MotorSts::Mode::POSITIONAL},
        {2,1869,830 ,3200,true ,MotorSts::Mode::POSITIONAL},
        {3,2214,883 ,3080,true ,MotorSts::Mode::POSITIONAL},
        {4,1985,862 ,2900,true ,MotorSts::Mode::POSITIONAL},
        {5,2057,300 ,3980,true ,MotorSts::Mode::POSITIONAL},
        {6,1412,1400,2870,false,MotorSts::Mode::POSITIONAL},
        },

        controller
        {
        {11,2035,691 ,3423,false,MotorSts::Mode::POSITIONAL},
        {12,1906,849 ,3254,true ,MotorSts::Mode::POSITIONAL},
        {13,2457,1131,3330,true ,MotorSts::Mode::POSITIONAL},
        {14,1671,480 ,2762,true ,MotorSts::Mode::POSITIONAL},
        {15,1503,132 ,3965,true ,MotorSts::Mode::POSITIONAL},
        {16,1702,1668,3015,false,MotorSts::Mode::POSITIONAL},
        }
    {}
    void DeSolve() const;
    void ControlLoop();
    void TransmitBusControlCmd();
    void DisableAll();

    void GetDataFromRc();
    bool Solve();
    void SolveEnd();
    void Follow();

    // float target_x = 110.0f, target_y = -110.0f, target_theta = -1.0f;
    // float target_x = 40.0f, target_y = 40.0f, target_theta = -1.0f;
    float target_x = 20.0f, target_y = 45.0f, target_theta = -0.8f;
    float target_x_last = 20.0f, target_y_last = 45.0f, target_theta_last = -0.8f;
    float target_wrist = 0.485f, target_gripper = 0;
    float target_yaw_ = 0.0f;

    const float l1 = 115,l2 = 135,l3 = 165;
    MotorSts motors[6];
    MotorSts controller[6];
};
