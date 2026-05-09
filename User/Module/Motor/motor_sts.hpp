//
// Created by Glucose_carbide on 2026-05-02.
//

#pragma once

#include "utils_math.hpp"

class MotorSts
{
    friend class LekiwiArm;         // TODO 完工后去掉
    friend class LekiwiChassis;     // TODO 完工后去掉

    static constexpr uint8_t MAX_MOTORS_COUNT   = 20;       // 最大电机数量(多了可能加重负担，但实际应该不太影响)
    static constexpr uint8_t MAX_BUF_LEN        = 32;       // 每个电机私有缓冲区长度(不要太大，大了会显著加重堆栈/RAM负担)
    static constexpr uint16_t TIMEOUT_TICK      = 100;      // 超时阈值(ms)
    static constexpr bool DEBUG_MODE            = false;    // 调试模式(开启后将会在串口重定向中输出解包数据)

    static constexpr uint16_t ECD_RESOLUTION    = 4096;     // 不能改，除非协议变动 // 电机编码器分辨率(刻度/圈)
    static constexpr uint8_t MAX_MOTOR_ID       = 0xFD;     // 不能改，除非协议变动 // 最大电机的ID
public:
    static constexpr bool USE_MOTOR_POS_LIMIT   = true;     // 是否采用电机的编码器角度值，设为true后上电会读取已注册在 Init 前的所有电机的角度限制
    typedef enum : uint8_t
    {
        PING        = 0x01, // 查询电机状态
        READ        = 0x02, // 读电机寄存器值
        WRITE       = 0x03, // 写入电机寄存器
        RESET       = 0x06, // 重置寄存器
        ASYN_WRITE  = 0x04, // 异步写写入
        ASYN_ACTION = 0x05, // 异步写执行
        SYN_READ    = 0x82, // 同步读
        SYN_WRITE   = 0x83, // 同步写
    } Command;

    typedef enum : uint8_t
    {
        OFF         = 0,    // 失能电机
        ON          = 1,    // 使能电机
        DAMPING     = 2,    // 阻尼输出(被动模式)
        SET_2048    = 128,  // 设置当前位置为编码器2048点(二义性?!)
    } TorqueSwitch;

    typedef enum : uint8_t
    {
        POSITIONAL  = 0,
        VELOCITY    = 1,
        PWM         = 2,
        RAMP        = 3
    } Mode;

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

private:
    typedef enum : uint8_t
    {
        DUMMY       = 0x00, // 占位符
        MASTER_ID   = 0xFE, // 广播ID
        ILLEGAL_ID  = 0xFF, // 填充ID用，不可能是正常的电机ID
    } Special;

public:
    MotorSts(uint8_t ID, uint16_t zero_point, uint16_t min_angle, uint16_t max_angle, bool reversed, Mode mode);
    ~MotorSts();

    void UnpackData();
    void AddReadReg(REG r);
    void AddReadRangeByCount(REG s, uint8_t c);
    void AddReadRange(REG s, REG e);
    void SetReadRange(REG s, REG e);
    void TransmitReadCommand();
    void TransmitWriteCommand(REG r, uint16_t v) const;

    static void RxCallback(const uint8_t* data);
    static void UpdateAll();
    static void ControlAll();
    static void ReadAll(REG s, REG e);
    static void WriteAll(REG r, uint16_t v);
    static void Init();
    [[nodiscard]] static bool IsConnected() { return connected_; }

    void SetSoftTargetPos_Ecd(int16_t t);
    void SetSoftTargetPos_Rad(const float t)    { SetSoftTargetPos_Ecd(Rad2Ecd_Pos(t)); }
    void SetSoftTargetPos_One(const float t)    { SetSoftTargetPos_Ecd(One2Ecd_Pos(t)); }
    void SetSoftTargetLoad_Ecd(const int16_t t) { soft_target_load_ecd_ = t; target_load_ecd_ = is_reversed_ ? -t : t; is_param_set_ = true; }
    void SetSoftTargetLoad_One(const float t)   { SetSoftTargetLoad_Ecd(One2Ecd_Load(t)); }
    void SetSoftTargetVel_Ecd(const int16_t t)  { soft_target_vel_ecd_ = t; target_vel_ecd_ = is_reversed_ ? -t : t; is_param_set_ = true; }
    void SetSoftTargetVel_Rad(const float t)    { SetSoftTargetVel_Ecd(Rad2Ecd_Pos(t)); }

    [[nodiscard]] bool IsSafePos_Ecd(const int16_t p) const { return utils::IsBetween(static_cast<int16_t>(is_reversed_ ? -p : p), soft_min_pos_ecd_, soft_max_pos_ecd_, static_cast<int16_t>(10)); }
    [[nodiscard]] bool IsSafePos_Rad(const float p)   const { return IsSafePos_Ecd(Rad2Ecd_Pos(p)); }

    [[nodiscard]] bool IsReversed() const { return is_reversed_; }
    [[nodiscard]] bool IsOnline() const { return is_online_; }
    [[nodiscard]] uint8_t GetID() const { return ID_; }

    [[nodiscard]] uint16_t GetHardTargetVel_Ecd() const { return target_vel_ecd_; }
    [[nodiscard]] uint16_t GetHardTargetPos_Ecd() const { return target_pos_ecd_; }
    [[nodiscard]] uint16_t GetHardTargetLoad_Ecd()const { return target_load_ecd_; }
    [[nodiscard]] uint16_t GetHardPos_Raw()       const { return pos_ecd_; }
    [[nodiscard]] uint16_t GetHardVel_Raw()       const { return vel_ecd_; }
    [[nodiscard]] uint16_t GetHardLoad_Raw()      const { return load_ecd_; }

    [[nodiscard]] int16_t  GetSoftTargetVel_Ecd() const { return soft_target_vel_ecd_; }
    [[nodiscard]] float    GetSoftTargetVel_Rad() const { return Ecd2Rad_Pos(GetSoftTargetVel_Ecd()); }
    [[nodiscard]] int16_t  GetSoftTargetPos_Ecd() const { return soft_target_pos_ecd_; }
    [[nodiscard]] float    GetSoftTargetPos_Rad() const { return Ecd2Rad_Pos(GetSoftTargetPos_Ecd()); }
    [[nodiscard]] float    GetSoftTargetPos_One() const { return Ecd2One_Pos(GetSoftTargetPos_Ecd()); }
    [[nodiscard]] int16_t  GetSoftTargetLoad_Ecd()const { return soft_target_load_ecd_; }
    [[nodiscard]] float    GetSoftTargetLoad_One()const { return Ecd2One_Load(GetSoftTargetLoad_Ecd()); }
    [[nodiscard]] int16_t  GetSoftVel_Ecd()       const { return soft_vel_ecd_; }
    [[nodiscard]] float    GetSoftVel_Rad()       const { return Ecd2Rad_Pos(GetSoftVel_Ecd()); }
    [[nodiscard]] int16_t  GetSoftPos_Ecd()       const { return soft_pos_ecd_; }
    [[nodiscard]] float    GetSoftPos_Rad()       const { return Ecd2Rad_Pos(GetSoftPos_Ecd()); }
    [[nodiscard]] float    GetSoftPos_One()       const { return Ecd2One_Pos(GetSoftPos_Ecd()); }
    [[nodiscard]] int16_t  GetSoftLoad_Ecd()      const { return load_ecd_; }
    [[nodiscard]] float    GetSoftLoad_One()      const { return Ecd2One_Load(GetSoftLoad_Ecd()); }


private:
    static int16_t  PackStsData(uint8_t L, uint8_t H, uint8_t s = 15);
    static uint16_t ConvertStsData(uint16_t d, uint8_t s = 15);
    static bool     IsLowByteReg(REG r);
    void Reload(uint32_t tick_now);
    void ClampPos() { target_pos_ecd_ = utils::Clamp(target_pos_ecd_, min_pos_ecd_, max_pos_ecd_); }
    [[nodiscard]] static int16_t  Rad2Ecd_Pos(const float v) { return static_cast<int16_t>(utils::Rad2Round(v) * ECD_RESOLUTION); }
    [[nodiscard]] static float    Ecd2Rad_Pos(const int16_t v) { return utils::Round2Rad(v) / static_cast<float>(ECD_RESOLUTION); }
    [[nodiscard]] static int16_t  One2Ecd_Load(const float v) { return static_cast<int16_t>(v * 1000.0f); }
    [[nodiscard]] static float    Ecd2One_Load(const int16_t v) { return static_cast<float>(v) / 1000.0f; }

    [[nodiscard]] float   Ecd2One_Pos(const int16_t v) const { return utils::Map(v, soft_min_pos_ecd_, soft_max_pos_ecd_, 0.0f, 1.0f); }
    [[nodiscard]] int16_t One2Ecd_Pos(const float v) const { return static_cast<int16_t>(utils::Map(v, 0.0f, 1.0f, soft_min_pos_ecd_, soft_max_pos_ecd_)); }

    uint8_t ID_;
    const bool is_reversed_; const Mode mode_;
    const uint16_t zero_point_ecd_;
    uint16_t min_pos_ecd_;  int16_t soft_min_pos_ecd_;
    uint16_t max_pos_ecd_;  int16_t soft_max_pos_ecd_;

    uint16_t pos_ecd_{2048};    int16_t soft_pos_ecd_{2048};
    uint16_t vel_ecd_{0};       int16_t soft_vel_ecd_{0};
    int16_t load_ecd_{0};       int16_t soft_load_ecd_{0};
    uint8_t volt_ecd_{0};
    uint8_t temp_ecd_{0};
    int16_t cur_ecd_{0};

    uint16_t target_pos_ecd_{0};        int16_t soft_target_pos_ecd_{0};
    uint16_t target_load_ecd_{0};       int16_t soft_target_load_ecd_{0};
    uint16_t target_vel_ecd_{32767};    int16_t soft_target_vel_ecd_{32767};
    uint16_t max_load_ecd{1000};

    uint32_t last_ack_tick_{0};
    uint8_t cmd_l_{0xFF}, cmd_h_{0xFF};   // 设置读取寄存器时被置位，读取指令发送时被复位
    uint8_t ack_l_{0xFF}, ack_h_{0xFF};   // 读取指令发送时被上面的置位，解析完成后被复位
    uint8_t status_{0};
    bool is_online_{false};
    bool is_param_set_{false};     // 设置参数时置位以被ControlAll调用发送，发送后复位
    bool received_pack_{false};    // 有匹配该ID的回调时置位以执行Unpack指令(即使未成功写入也会置位防止卡死)，解包返回帧后复位
    bool in_use_{false};           // 发送读指令后置位以防止回调完成之前被再次执行读指令，解包返回帧后复位
    bool is_unpacking_{false};     // 开始解包时置位以阻止中断回调写入数据，解包完成后复位

    uint8_t rx_buffer_[MAX_BUF_LEN]{};

    static inline uint8_t motors_count_{0};                 // 电机数量，构造与析构时被修改
    static inline MotorSts* motors_[MAX_MOTORS_COUNT]{};    // 静态电机列表，负责维护并批量处理所有电机
    static inline uint8_t motors_idx_[MAX_MOTOR_ID + 1]{};  // 电机ID索引列表，记录每个ID的电机在motors_中的索引(没错，有相同ID的电机的话就是UB)
    static inline uint32_t last_ack_all_{0};                // 电机上一次应答包时间戳，在任意电机传数据时更新
    static inline volatile bool connected_{false};          // 电机在线标志，当所有电机都超时时置为false(检查一下电开没开?)
};
