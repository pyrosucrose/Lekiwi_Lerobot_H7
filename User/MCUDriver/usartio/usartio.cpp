//
// Created by Glucose_carbide on 25-8-8.
//

// =============================== 引入头文件 ===============================
#include "usartio.hpp"
#include "dma.h"
#include <cstdarg>
#include <cstdio>

#include "buzzer.hpp"
#include "delay.hpp"
#include "remote.hpp"
#include "template.hpp"
#include "robot.hpp"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ==================================
uint8_t uart1_rx_buffer[UART1_RX_BUFFER_SIZE] = {};
uint8_t uart1_tx_buffer[UART1_TX_BUFFER_SIZE] = {};
uint8_t uart5_rx_buffer[UART5_RX_BUFFER_SIZE] = {};
uint8_t uart5_tx_buffer[UART5_TX_BUFFER_SIZE] = {};
uint8_t uart_sts_rx_buffer[UART_STS_RX_BUFFER_SIZE] = {};
uint8_t uart_sts_tx_buffer[UART_STS_TX_BUFFER_SIZE] = {0xFF, 0xFF};

inline constexpr uint8_t  UART_TX_BUFFER_COUNT = 32;
inline constexpr uint16_t UART_TX_BUFFER_SIZE = 64;

struct
{
    uint8_t buffers[UART_TX_BUFFER_COUNT][UART_TX_BUFFER_SIZE] = {};
    uint16_t lengths[UART_TX_BUFFER_COUNT] = {}; // 记录每条消息的长度
    uint8_t write_idx = 0;
    uint8_t transmit_idx = 0;
    bool is_transmitting = false;
} tx_ring;

// =============================== 函数实现 ===============================
uint8_t TryStartTransmit();

void Usartio_Init()
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart5, uart5_rx_buffer, UART5_RX_BUFFER_SIZE);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart10, uart_sts_rx_buffer, UART_STS_RX_BUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(&hdma_uart5_rx, DMA_IT_HT);
    __HAL_DMA_DISABLE_IT(&hdma_usart10_rx, DMA_IT_HT);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART1)
    {
        if (Size > 0)
        {
        }
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart1_rx_buffer, UART1_RX_BUFFER_SIZE);
    }

    else if (huart->Instance == USART10)
    {
        // auto s = Delay::GetTimeStamp();
        if (Size > 0)
        {
            MotorSts::RxCallback(uart_sts_rx_buffer);
        }
        HAL_UARTEx_ReceiveToIdle_DMA(&huart10, uart_sts_rx_buffer, UART_STS_RX_BUFFER_SIZE);
        // usart_printf("%f\n",Delay::CalculateInterval_us(s));
    }

    else if (huart->Instance == UART5)
    {
        if (Size > 0)
        {
            rc_data.RcUpdateValue(uart5_rx_buffer);
        }
        HAL_UARTEx_ReceiveToIdle_DMA(&huart5, uart5_rx_buffer, UART5_RX_BUFFER_SIZE);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        tx_ring.transmit_idx = (tx_ring.transmit_idx + 1) % UART_TX_BUFFER_COUNT;
        tx_ring.is_transmitting = false;
        TryStartTransmit();
    }
}

uint8_t TryStartTransmit()
{
    if (tx_ring.is_transmitting || tx_ring.write_idx == tx_ring.transmit_idx)
        return USART_PRINTF_WAITING;

    const uint8_t current_transmit_idx = tx_ring.transmit_idx;

    if (const HAL_StatusTypeDef status = HAL_UART_Transmit_DMA(&huart1,
                                tx_ring.buffers[current_transmit_idx],
                                tx_ring.lengths[current_transmit_idx]); status == HAL_OK)
        tx_ring.is_transmitting = true;
    else
        Buzzer::AddToNoteTrack(3000, 2000);
    return USART_PRINTF_TRANSMIT;
}

uint8_t usart_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    const uint8_t next_write_idx = (tx_ring.write_idx + 1) % UART_TX_BUFFER_COUNT;

    if (next_write_idx == tx_ring.transmit_idx)
    {
        va_end(args);
        return USART_PRINTF_BUFFER_OVERFLOW;
    }

    const uint32_t len = vsnprintf((char*)tx_ring.buffers[tx_ring.write_idx], UART_TX_BUFFER_SIZE, format, args);
    va_end(args);

    if (len <= 0) return USART_PRINTF_WRONG_LEN;

    tx_ring.lengths[tx_ring.write_idx] = static_cast<uint16_t>(len);
    tx_ring.write_idx = next_write_idx;

    return TryStartTransmit();
}
