//
// Created by Glucose_carbide on 2026-05-02.
//

#include "robot.hpp"
#include "main.h"
#include "remote.hpp"

void RobotLekiwi::ControlLoop()
{
    chassis.ControlLoop();
    arm.ControlLoop();
}

void RobotLekiwi::SendControl()
{
    static bool read_motor_info = false;
    read_motor_info = !read_motor_info;

    if (read_motor_info)
        MotorSts::ReadAll(MotorSts::REG::NOW_POS_L, MotorSts::REG::NOW_TEMP);
    else
        if (rc_data.IsRcOnline() && rc_data.GetRcSwitchC() != eRemoteSwitchValue::HIGH)
            MotorSts::ControlAll();
        else
            MotorSts::WriteAll(MotorSts::REG::TORQUE_SWITCH, MotorSts::TorqueSwitch::OFF);
}
