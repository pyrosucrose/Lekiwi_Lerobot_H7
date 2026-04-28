//
// Created by Glucose_carbide on 25-8-8.
//

// =============================== 引入头文件 ===============================
#include "arm.hpp"
#include "math.hpp"
#include "remote.hpp"
#include "usartio.hpp"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ==================================

// =============================== 函数实现 ===============================
void cArm::GetDataFromRc()
{
    if (rc_data.GetRcSwitchB() == LOW)
    {
        target_x_last = target_x;
        target_y_last = target_y;
        target_theta_last = target_theta;

        target_x += rc_data.GetRcLeftVertical() * 0.7f;
        target_y += rc_data.GetRcRightVertical() * 0.7f;

        target_wrist -= rc_data.GetRcRightHorizontal() * 10.0f;
        target_wrist = Clamp(target_wrist, 300, 3980);
        target_pos_ecd[4] = static_cast<uint16_t>(target_wrist);

        target_x = Clamp(target_x, -200, 300);
        target_y = Clamp(target_y, -200, 300);
    }
    else if (rc_data.GetRcSwitchA() == LOW)
    {
        target_x_last = target_x;
        target_y_last = target_y;
        target_theta_last = target_theta;

        target_x += rc_data.GetRcLeftVertical() * cosf(target_theta) * 0.7f; // rc_data.GetRcLeftVertical();
        target_y += rc_data.GetRcLeftVertical() * sinf(target_theta) * 0.7f; // rc_data.GetRcRightVertical()
        target_theta += rc_data.GetRcRightVertical() / 100.0f;

        target_wrist -= rc_data.GetRcRightHorizontal() * 10.0f;
        target_wrist = Clamp(target_wrist, 300, 3980);
        target_pos_ecd[4] = static_cast<uint16_t>(target_wrist);

        target_x = Clamp(target_x, -200, 300);
        target_y = Clamp(target_y, -200, 300);
        target_theta = Clamp(target_theta, -M_PI * 3 / 5, M_PI * 3 / 4);
    }
    else
    {
        target_theta += rc_data.GetRcRightVertical() / 100.0f;
        target_theta = Clamp(target_theta, -M_PI * 3 / 5, M_PI * 3 / 4);
    }
    target_gripper = Map(rc_data.GetRcKnobLeft(), -783.0, 783.0, 1400, 2870);
    target_pos_ecd[5] = static_cast<uint16_t>(target_gripper);
}

bool cArm::SolveArm()
{
    auto Rad2Ecd = [this] (const float agl, const uint8_t i)
    {
        return motors[i].is_reversed ?
            static_cast<int16_t>(motors[i].zero_point_ecd - static_cast<int16_t>(agl * 4096.0 / (2 * M_PI))):
            static_cast<int16_t>(motors[i].zero_point_ecd + static_cast<int16_t>(agl * 4096.0 / (2 * M_PI)));
    };

    const float X = target_x;// - l3 * cosf(target_theta);
    const float Y = target_y;// - l3 * sinf(target_theta);
    const float R = X*X + Y*Y >= 1.0f ? sqrtf(X*X + Y*Y) : 0.0f;
    const float K = (l2*l2 + X*X + Y*Y - l1*l1) / (2.0f*l2);
    if (R < 1e-6f) return false;
    float K_R = K / R;
    if (fabsf(K_R) <= 1+1e-6f) K_R = Clamp(K_R, -1.0f, 1.0f);
    else return false;
    const float phi = atan2f(Y, X);

    // β = φ ± cos(K/R)
    // α = atan2(l2*cos(β) - X, Y - l2*sin(β))
    // a = α; a + b = β; a + b + c = θ
    const float beta_1 = phi - acosf(K_R);
    const float beta_2 = phi + acosf(K_R);
    const float a1 = atan2f(l2 * cosf(beta_1) - X, Y - l2 * sinf(beta_1));
    const float a2 = atan2f(l2 * cosf(beta_2) - X, Y - l2 * sinf(beta_2));
    const float b1 = Round_v(beta_1 - a1, 2 * M_PI);
    const float b2 = Round_v(beta_2 - a2, 2 * M_PI);
    const float c1 = Round_v(target_theta - beta_1, 2 * M_PI);
    const float c2 = Round_v(target_theta - beta_2, 2 * M_PI);

    const int16_t a_ecd_1 = Rad2Ecd(a1, 1);
    const int16_t a_ecd_2 = Rad2Ecd(a2, 1);
    const int16_t b_ecd_1 = Rad2Ecd(b1, 2);
    const int16_t b_ecd_2 = Rad2Ecd(b2, 2);
    const int16_t c_ecd_1 = Rad2Ecd(c1, 3);
    const int16_t c_ecd_2 = Rad2Ecd(c2, 3);
    // usart_printf("%d,%d,%d,%d,%d,%d\n",a_ecd_1,b_ecd_1,c_ecd_1,a_ecd_2,b_ecd_2,c_ecd_2);

    if (IsBetween(a_ecd_2, motors[1].min_angle_ecd, motors[1].max_angle_ecd, 10.0f) &&
        IsBetween(b_ecd_2, motors[2].min_angle_ecd, motors[2].max_angle_ecd, 10.0f) &&
        IsBetween(c_ecd_2, motors[3].min_angle_ecd, motors[3].max_angle_ecd, 10.0f))
    {
        target_pos_ecd[1] = a_ecd_2;
        target_pos_ecd[2] = b_ecd_2;
        target_pos_ecd[3] = c_ecd_2;
        return true;
    }
    if (IsBetween(a_ecd_1, motors[1].min_angle_ecd, motors[1].max_angle_ecd, 10.0f) &&
        IsBetween(b_ecd_1, motors[2].min_angle_ecd, motors[2].max_angle_ecd, 10.0f) &&
        IsBetween(c_ecd_1, motors[3].min_angle_ecd, motors[3].max_angle_ecd, 10.0f))
    {
        target_pos_ecd[1] = a_ecd_1;
        target_pos_ecd[2] = b_ecd_1;
        target_pos_ecd[3] = c_ecd_1;
        return true;
    }
    return false;
}

void cArm::SolveEnd()
{

}

void cArm::DeSolveArm() const
{
    auto Ecd2Rad = [this] (const int16_t ecd, const size_t i)
    {
        return motors[i].is_reversed ?
            static_cast<float>(static_cast<float>(motors[i].zero_point_ecd - ecd) * 2.0f * M_PI / 4096.0f) :
            static_cast<float>(static_cast<float>(ecd - motors[i].zero_point_ecd) * 2.0f * M_PI / 4096.0f);
    };

    const float a_rad = Ecd2Rad(motors[1].pos_ecd, 1);
    const float b_rad = Ecd2Rad(motors[2].pos_ecd, 2);
    const float c_rad = Ecd2Rad(motors[3].pos_ecd, 3);

    const float theta = a_rad + b_rad + c_rad;
    const float x = -l1 * sinf(a_rad) + l2 * cosf(a_rad + b_rad);// + l3 * cosf(theta);
    const float y =  l1 * cosf(a_rad) + l2 * sinf(a_rad + b_rad);// + l3 * sinf(theta);

    usart_printf("%.1f,%.1f,%.1f,%.1f,%.1f,%.1f\n",target_x,target_y,Rad2Degree(target_theta),x,y,Rad2Degree(theta));
}

void cArm::TransmitBusControlCmd()
{   // 帧头
    uint8_t idx = 0;
    uart10_tx_buffer[idx++] = 0xFF;
    uart10_tx_buffer[idx++] = 0xFF;
    uart10_tx_buffer[idx++] = 0xFE;
    uart10_tx_buffer[idx++] = 0x16;
    uart10_tx_buffer[idx++] = 0x83;
    uart10_tx_buffer[idx++] = static_cast<uint8_t>(cMotorSts::REG::TARGET_POSITION_L);
    uart10_tx_buffer[idx++] = 0x02;
    // 命令
    for (const auto& motor : motors)
    {
        uint16_t val = motor.target_pos;
        if ((val & 0x8000) != 0) val = static_cast<int16_t>(-(val & 0x7FFF));

        uart10_tx_buffer[idx++] = motor.ID;
        uart10_tx_buffer[idx++] = val;
        uart10_tx_buffer[idx++] = val >> 8;
    }
    // 校验和
    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < idx; i++)
        check_sum += uart10_tx_buffer[i];
    check_sum = ~check_sum;
    uart10_tx_buffer[idx++] = check_sum;
    // 发送
    HAL_UART_Transmit_DMA(&huart10, uart10_tx_buffer, idx);
}

void cArm::DisableAll()
{   // 帧头
    uint8_t idx = 0;
    uart10_tx_buffer[idx++] = 0xFF;
    uart10_tx_buffer[idx++] = 0xFF;
    uart10_tx_buffer[idx++] = 0xFE;
    uart10_tx_buffer[idx++] = 0x10;
    uart10_tx_buffer[idx++] = 0x83;
    uart10_tx_buffer[idx++] = static_cast<uint8_t>(cMotorSts::REG::TORQUE_SWITCH);
    uart10_tx_buffer[idx++] = 0x01;
    // 命令
    for (const auto& motor : motors)
    {
        uart10_tx_buffer[idx++] = motor.ID;
        uart10_tx_buffer[idx++] = 2;
    }
    // 校验和
    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < idx; i++)
        check_sum += uart10_tx_buffer[i];
    // check_sum = ~check_sum;
    uart10_tx_buffer[idx++] = ~check_sum;
    // 发送
    HAL_UART_Transmit_DMA(&huart10, uart10_tx_buffer, idx);
}

void cArm::ControlLoop()
{
    if (rc_data.IsRcOnline())
    {
        GetDataFromRc();
        if (!SolveArm())
        {
            target_x = target_x_last;
            target_y = target_y_last;
            target_theta = target_theta_last;
        }
        SolveEnd();
        for (uint8_t i = 0; i < 6; i++)
            motors[i].target_pos = target_pos_ecd[i];
        // DeSolveArm();
    }
}
