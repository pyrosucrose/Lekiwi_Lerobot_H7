//
// Created by Glucose_carbide on 25-8-8.
//

#pragma once

// =============================== 调用库 ===============================
#include <cstdint>

#include "motor_sts.hpp"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ===============================
class cMotorSts;
// =============================== 函数声明 ===============================
// void STS_TransmitWriteCommand(uint8_t ID, const cMotorSts& motor);
void STS_TransmitBusCommand();

// =============================== 类声明 ===============================
class cMotorSts
{
public:
    enum class REG : uint8_t
    {
        // EPROM
        FIRMWARE_MAJOR_VERSION   = 0x00, // R
        FIRMWARE_MINOR_VERSION   = 0x01, // R
        END_MARKER               = 0x02, // R
        SERVO_MAJOR_VERSION      = 0x03, // R
        SERVO_MINOR_VERSION      = 0x04, // R

        ID                       = 0x05, // RW
        BAUD_RATE                = 0x06, // RW
        RETURN_DELAY             = 0x07, // RW
        RESPONSE_STATUS_LEVEL    = 0x08, // RW
        MIN_ANGLE_LIMIT_L        = 0x09, // RW
        MIN_ANGLE_LIMIT_H        = 0x0A, // RW
        MAX_ANGLE_LIMIT_L        = 0x0B, // RW
        MAX_ANGLE_LIMIT_H        = 0x0C, // RW
        MAX_TEMPERATURE          = 0x0D, // RW
        MAX_VOLTAGE              = 0x0E, // RW
        MIN_VOLTAGE              = 0x0F, // RW
        MAX_TORQUE_L             = 0x10, // RW
        MAX_TORQUE_H             = 0x11, // RW
        PHASE                    = 0x12, // RW
        UNLOAD_CONDITION         = 0x13, // RW
        LED_ALARM_CONDITION      = 0x14, // RW
        POS_P_GAIN               = 0x15, // RW
        POS_D_GAIN               = 0x16, // RW
        POS_I_GAIN               = 0x17, // RW
        MIN_START_TORQUE         = 0x18, // RW
        INTEGRAL_LIMIT           = 0x19, // RW
        CW_DEAD_ZONE             = 0x1A, // RW
        CCW_DEAD_ZONE            = 0x1B, // RW
        PROTECTION_CURRENT_L     = 0x1C, // RW
        PROTECTION_CURRENT_H     = 0x1D, // RW
        ANGLE_RESOLUTION         = 0x1E, // RW
        POSITION_CORRECTION_L    = 0x1F, // RW
        POSITION_CORRECTION_H    = 0x20, // RW
        OPERATION_MODE           = 0x21, // RW
        PROTECTION_TORQUE        = 0x22, // RW
        PROTECTION_TIME          = 0x23, // RW
        OVERLOAD_TORQUE          = 0x24, // RW
        SPEED_P_GAIN             = 0x25, // RW
        OVERCURRENT_PROTECT_TIME = 0x26, // RW
        SPEED_I_GAIN             = 0x27, // RW

        // SRAM
        TORQUE_SWITCH            = 0x28, // RW
        ACCELERATION             = 0x29, // RW
        TARGET_POSITION_L        = 0x2A, // RW
        TARGET_POSITION_H        = 0x2B, // RW
        MOVING_TIME_L            = 0x2C, // RW
        MOVING_TIME_H            = 0x2D, // RW
        TARGET_SPEED_L           = 0x2E, // RW
        TARGET_SPEED_H           = 0x2F, // RW
        TORQUE_LIMIT_L           = 0x30, // RW
        TORQUE_LIMIT_H           = 0x31, // RW
        LOCK                     = 0x37, // RW

        NOW_POS_L                = 0x38, // R
        NOW_POS_H                = 0x39, // R
        NOW_SPEED_L              = 0x3A, // R
        NOW_SPEED_H              = 0x3B, // R
        NOW_LOAD_L               = 0x3C, // R
        NOW_LOAD_H               = 0x3D, // R
        NOW_VOLT                 = 0x3E, // R
        NOW_TEMP                 = 0x3F, // R
        ASYNCHRONOUS_WRITE       = 0x40, // R
        STATUS                   = 0x41, // R
        IS_MOVING                = 0x42, // R
        NOW_CURRENT_L            = 0x45, // R
        NOW_CURRENT_H            = 0x46, // R
    };

    enum class CALLBACK_TYPE : uint8_t
    {
        STATE_PARAMS = 0x00,
        CONFIG_PARAMS,
        WRONG_ID,
        WRONG_HEAD,     // Fatal fault
        WRONG_LENGTH,
        WRONG_CHECKSUM, // Fatal fault
    };
    cMotorSts(const uint8_t ID, const uint16_t zero_point, const uint16_t min_angle, const uint16_t max_angle, const bool reversed = false) :
        ID(ID), zero_point_ecd(zero_point), min_angle_ecd(min_angle), max_angle_ecd(max_angle), is_reversed(reversed) {}
    ~cMotorSts() = default;

    void AddReadReg(REG reg);
    void AddReadRangeByCount(REG start, uint8_t count);
    void AddReadRange(REG start, REG end);
    void TransmitReadCommand() const;
    void TransmitWriteCommand(REG reg, int16_t value) const;
    CALLBACK_TYPE RxCallback(const uint8_t* data);

    [[nodiscard]] static inline bool Is16BitWriteReg(REG reg);

    const uint16_t zero_point_ecd;
    const uint16_t min_angle_ecd;
    const uint16_t max_angle_ecd;
    const bool is_reversed;

    uint8_t ID;
    uint8_t read_reg_l = 0xFF, read_reg_h = 0x00;
    uint8_t write_reg_l = 0xFF, write_reg_h = 0x00;
    uint8_t status = 0;
    bool error = false;
    bool enabled = false;

    int16_t pos_ecd = 0;
    int16_t vel_ecd = 0;
    int16_t load_ecd = 0;
    uint8_t volt_ecd = 0;
    uint8_t temp_ecd = 0;
    int16_t cur_ecd = 0;

    int16_t target_vel = 0;
    int16_t target_pos = 0;
};
