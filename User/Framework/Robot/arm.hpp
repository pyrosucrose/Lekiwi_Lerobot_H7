//
// Created by Glucose_carbide on 25-8-8.
//

#ifndef ARM_HPP
#define ARM_HPP

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
        {1,2035,720,3440,true},
        {2,1869,830,3200,true},
        {3,2214,883,3080,true},
        {4,1985,862,2900,true},
        {5,2057,300,3980,true},
        {6,1412,1400,2870,false},
        }
    {}
    bool RxCallback(const uint8_t* data);
    void DeSolveArm(int16_t p1, int16_t p2, int16_t p3) const;
    void ControlLoop();
    void TransmitBusControlCmd();

    void GetDataFromRc();
    bool SolveArm();

    float target_x = 110.0f, target_y = -110.0f, target_theta = -1.0f;
    // float target_x = 40.0f, target_y = 40.0f, target_theta = -1.0f;
    uint16_t target_pos_ecd[6] = {};
    const float l1 = 115,l2 = 135,l3 = 165;
    cMotorSts motors[6];
};

#endif // ARM_HPP
