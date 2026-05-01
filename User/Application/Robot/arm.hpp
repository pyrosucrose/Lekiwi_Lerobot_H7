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
class cArm
{
    friend class cRobot;
public:
    cArm() :
        motors{
        // 从臂
        {1,2035,720,3440,true},
        {2,1869,830,3200,true},
        {3,2214,883,3080,true},
        {4,1985,862,2900,true},
        {5,2057,300,3980,false},
        {6,1412,1400,2870,false},
        //主臂
        // {1,2035,720,3440,false},
        // {2,1906,830,3200,true},
        // {3,2457,883,3080,true},
        // {4,1671,862,2900,true},
        // {5,1503,300,3980,false},
        // {6,1702,1400,2870,false},
        }
    {}
    void DeSolve() const;
    void ControlLoop();
    void TransmitBusControlCmd();
    void DisableAll();

    void GetDataFromRc();
    bool Solve();
    void SolveEnd();

    // float target_x = 110.0f, target_y = -110.0f, target_theta = -1.0f;
    // float target_x = 40.0f, target_y = 40.0f, target_theta = -1.0f;
    float target_x = 20.0f, target_y = 45.0f, target_theta = -0.8f;
    float target_x_last = 20.0f, target_y_last = 45.0f, target_theta_last = -0.8f;
    float target_wrist = 0.485f, target_gripper = 0;
    float target_yaw_ = 0.0f;

    const float l1 = 115,l2 = 135,l3 = 165;
    MotorSts motors[6];
};
