//
// Created by Glucose_carbide on 2026-05-02.
//

#pragma once

#include "arm.hpp"
#include "chassis.hpp"

class RobotLekiwi
{
public:
    void ControlLoop();
    static void SendControl();

    LekiwiArm arm;
    LekiwiChassis chassis;

};

inline RobotLekiwi robot_lekiwi;
