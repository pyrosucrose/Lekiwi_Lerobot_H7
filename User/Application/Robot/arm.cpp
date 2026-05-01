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
    target_x_last = target_x;
    target_y_last = target_y;
    target_theta_last = target_theta;

    target_gripper = Map(rc_data.GetRcKnobLeft(), -783.0, 783.0, 0.0f, 1.0f);

    if (rc_data.GetRcSwitchC() == eRemoteSwitchValue::LOW)
    {
        target_x = target_x_last = 20.0f;
        target_y = target_y_last = 45.0f;
        target_theta = target_theta_last = -0.8f;
        target_yaw_ = 0.0f;
        target_wrist = 0.485f;
        return;
    }

    const bool is_mode_a = (rc_data.GetRcSwitchA() == eRemoteSwitchValue::LOW);
    const bool is_mode_b = (rc_data.GetRcSwitchB() == eRemoteSwitchValue::LOW);

    if (is_mode_a || is_mode_b)
    {
        target_wrist += rc_data.GetRcRightHorizontal() * 2e-3f;
        target_wrist = Clamp(target_wrist, 0.0f, 1.0f);
        target_yaw_ += rc_data.GetRcLeftHorizontal() * 1e-2f;
    }

    if (is_mode_b)
    {
        target_x += rc_data.GetRcLeftVertical() * 0.7f;
        target_y += rc_data.GetRcRightVertical() * 0.7f;
    }
    else if (is_mode_a)
    {
        target_x += rc_data.GetRcLeftVertical() * cosf(target_theta) * 0.7f;
        target_y += rc_data.GetRcLeftVertical() * sinf(target_theta) * 0.7f;
        target_theta += rc_data.GetRcRightVertical() / 100.0f;
    }
    else
    {
        target_theta += rc_data.GetRcRightVertical() / 100.0f;
    }

    target_x = Clamp(target_x, -200.0f, 300.0f);
    target_y = Clamp(target_y, -200.0f, 300.0f);
    target_theta = Clamp(target_theta, -USER_PI * 3.0f / 5.0f, USER_PI * 3.0f / 4.0f);
}

bool cArm::Solve()
{
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

    motors[0].SetTargetPos_Rad(target_yaw_);
    if (motors[1].IsSafePos_Rad(a1) && motors[2].IsSafePos_Rad(b1) && motors[3].IsSafePos_Rad(c1))
    {
        motors[1].SetTargetPos_Rad(a1);
        motors[2].SetTargetPos_Rad(b1);
        motors[3].SetTargetPos_Rad(c1);
        motors[4].SetTargetPos_One(target_wrist);
        motors[5].SetTargetPos_One(target_gripper);
        return true;
    }
    if (motors[1].IsSafePos_Rad(a2) && motors[2].IsSafePos_Rad(b2) && motors[3].IsSafePos_Rad(c2))
    {
        motors[1].SetTargetPos_Rad(a2);
        motors[2].SetTargetPos_Rad(b2);
        motors[3].SetTargetPos_Rad(c2);
        motors[4].SetTargetPos_One(target_wrist);
        motors[5].SetTargetPos_One(target_gripper);
        return true;
    }
    return false;
}

void cArm::SolveEnd()
{

}

void cArm::DeSolve() const
{
    const float a = motors[1].GetSoftPos_Rad();
    const float b = motors[2].GetSoftPos_Rad();
    const float c = motors[3].GetSoftPos_Rad();

    const float theta = a + b + c;
    const float x = -l1 * sinf(a) + l2 * cosf(a + b);// + l3 * cosf(theta);
    const float y =  l1 * cosf(a) + l2 * sinf(a + b);// + l3 * sinf(theta);

    // usart_printf("%f,%f,%f\n",a,b,c);
    usart_printf("%.1f,%.1f,%.1f,%.1f,%.1f,%.1f\n",target_x,target_y,Rad2Degree(target_theta),x,y,Rad2Degree(theta));
}

void cArm::TransmitBusControlCmd()
{
    uint8_t idx = 0;
    uart_sts_tx_buffer[idx++] = 0xFF;
    uart_sts_tx_buffer[idx++] = 0xFF;
    uart_sts_tx_buffer[idx++] = MotorSts::Special::MASTER_ID;
    uart_sts_tx_buffer[idx++] = MotorSts::Special::DUMMY;
    uart_sts_tx_buffer[idx++] = MotorSts::Command::SYN_WRITE;
    uart_sts_tx_buffer[idx++] = static_cast<uint8_t>(MotorSts::REG::TARGET_POSITION_L);
    uart_sts_tx_buffer[idx++] = 0x02;

    for (const auto& motor : motors)
    {
        const uint16_t val = MotorSts::ConvertStsData(motor.GetHardTargetPos_Ecd());

        uart_sts_tx_buffer[idx++] = motor.GetID();
        uart_sts_tx_buffer[idx++] = val;
        uart_sts_tx_buffer[idx++] = val >> 8;
    }
    uart_sts_tx_buffer[3] = idx - 3;                          // FrameLength

    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < idx; i++)
        check_sum += uart_sts_tx_buffer[i];
    check_sum = ~check_sum;
    uart_sts_tx_buffer[idx++] = check_sum;

    HAL_UART_Transmit_DMA(&huart_sts, uart_sts_tx_buffer, idx);
}

void cArm::DisableAll()
{
    uint8_t idx = 0;
    uart_sts_tx_buffer[idx++] = 0xFF;
    uart_sts_tx_buffer[idx++] = 0xFF;
    uart_sts_tx_buffer[idx++] = MotorSts::Special::MASTER_ID;
    uart_sts_tx_buffer[idx++] = MotorSts::Special::DUMMY;
    uart_sts_tx_buffer[idx++] = MotorSts::Command::SYN_WRITE;
    uart_sts_tx_buffer[idx++] = static_cast<uint8_t>(MotorSts::REG::TORQUE_SWITCH);
    uart_sts_tx_buffer[idx++] = 0x01;

    for (const auto& motor : motors)
    {
        uart_sts_tx_buffer[idx++] = motor.GetID();
        uart_sts_tx_buffer[idx++] = 0;
    }
    uart_sts_tx_buffer[3] = idx - 3;                          // FrameLength

    uint8_t check_sum = 0;
    for (uint8_t i = 2; i < idx; i++)
        check_sum += uart_sts_tx_buffer[i];

    uart_sts_tx_buffer[idx++] = ~check_sum;

    HAL_UART_Transmit_DMA(&huart_sts, uart_sts_tx_buffer, idx);
}

void cArm::ControlLoop()
{
    if (rc_data.IsRcOnline())
    {
        GetDataFromRc();
        if (!Solve())
        {
            target_x = target_x_last;
            target_y = target_y_last;
            target_theta = target_theta_last;
        }
        SolveEnd();
        // DeSolve();
    }
}
