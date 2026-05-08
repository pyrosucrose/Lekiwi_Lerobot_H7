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
        {1,2035,720,3440,false},
        {2,1869,830,3200,true},
        {3,2214,883,3080,true},
        {4,1985,862,2900,true},
        {5,2057,300,3980,true},
        {6,1412,1400,2870,false},
        },

        controller
        {
        {11,2035,691,3423,false},
        {12,1906,849,3254,true},
        {13,2457,1131,3330,true},
        {14,1671,480,2762,true},
        {15,1503,132,3965,true},
        {16,1702,1668,3015,false},
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
