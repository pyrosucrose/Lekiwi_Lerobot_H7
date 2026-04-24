//
// Created by Glucose_carbide on 25-8-8.
//

// =============================== 引入头文件 ===============================
#include "usartio.hpp"
#include "dma.h"
#include <cstdarg>
#include <cstdio>

#include "remote.hpp"
#include "template.hpp"
#include "robot.hpp"

// =============================== 宏定义区 ===============================

// =============================== 变量区 ==================================
uint8_t uart1_rx_buffer[UART1_RX_BUFFER_SIZE] = {};
uint8_t uart1_tx_buffer[UART1_TX_BUFFER_SIZE] = {};
uint8_t uart5_rx_buffer[UART5_RX_BUFFER_SIZE] = {};
uint8_t uart5_tx_buffer[UART5_TX_BUFFER_SIZE] = {};
uint8_t uart10_rx_buffer[UART10_RX_BUFFER_SIZE] = {};
uint8_t uart10_tx_buffer[UART10_TX_BUFFER_SIZE] = {0xFF, 0xFF};

// =============================== 函数实现 ===============================
void Usartio_Init()
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart5, uart5_rx_buffer, UART5_RX_BUFFER_SIZE);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart10, uart10_rx_buffer, UART10_RX_BUFFER_SIZE);
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
        // Buzzer_ChangeFreqDuty(440);
        // usart_printf("%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X\n",
        //     uart2_rx_buffer[0],
        //     uart2_rx_buffer[1],
        //     uart2_rx_buffer[2],
        //     uart2_rx_buffer[3],
        //     uart2_rx_buffer[4],
        //     uart2_rx_buffer[5],
        //     uart2_rx_buffer[6],
        //     uart2_rx_buffer[7],
        //     uart2_rx_buffer[8],
        //     uart2_rx_buffer[9],
        //     uart2_rx_buffer[10],
        //     uart2_rx_buffer[11],
        //     uart2_rx_buffer[12],
        //     uart2_rx_buffer[13],
        //     uart2_rx_buffer[14],
        //     uart2_rx_buffer[15]
        //     );
        if (Size > 0)
        {
            robot_lekiwi.RxCallback(uart10_rx_buffer);
        }
        HAL_UARTEx_ReceiveToIdle_DMA(&huart10, uart10_rx_buffer, UART10_RX_BUFFER_SIZE);
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

void usart_printf(const char *format, ...)
{
	va_list args;
	uint32_t length = 0;

	va_start(args, format);
	length = vsnprintf((char *)uart1_tx_buffer, UART1_TX_BUFFER_SIZE, (const char *)format, args);
	va_end(args);

	HAL_UART_Transmit_DMA(&huart1, (uint8_t *)uart1_tx_buffer, length);
}