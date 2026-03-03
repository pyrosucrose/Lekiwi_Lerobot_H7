//
// Created by Glucose_carbide on 25-8-8.
//

#ifndef ROBOT_HPP
#define ROBOT_HPP

// =============================== 调用库 ===============================
#include "arm.hpp"
#include "chassis.hpp"
// =============================== 宏定义区 ===============================

// =============================== 变量区 ===============================

// =============================== 函数声明 ===============================

// =============================== 类声明 ===============================

class cRobot
{
public:
    void RxCallback(const uint8_t* data);
    void ControlLoop();

    cArm arm;
    cChassis chassis;

};

inline cRobot robot_lekiwi;

#endif // ROBOT_HPP
