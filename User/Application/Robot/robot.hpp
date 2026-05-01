//
// Created by Glucose_carbide on 25-8-8.
//

#pragma once
// =============================== 调用库 ===============================
#include "arm.hpp"
#include "chassis.hpp"
// =============================== 宏定义区 ===============================

// =============================== 变量区 ===============================

// =============================== 函数声明 ===============================

// =============================== 类声明 ===============================

class RobotLekiwi
{
public:
    void ControlLoop();
    void SendControl();

    LekiwiArm arm;
    LekiwiChassis chassis;

};

inline RobotLekiwi robot_lekiwi;
