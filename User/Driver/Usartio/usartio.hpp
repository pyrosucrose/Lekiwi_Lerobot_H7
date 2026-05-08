//
// Created by Glucose_carbide on 2026-05-02.
//

#pragma once

#include "usart.h"

inline constexpr uint16_t UART1_RX_BUFFER_SIZE = 255;
inline constexpr uint16_t UART1_TX_BUFFER_SIZE = 255;
inline constexpr uint16_t UART5_RX_BUFFER_SIZE = 255;
inline constexpr uint16_t UART5_TX_BUFFER_SIZE = 255;
inline constexpr uint16_t UART_STS_RX_BUFFER_SIZE = 255;
inline constexpr uint16_t UART_STS_TX_BUFFER_SIZE = 255;

typedef enum : uint8_t
{
    USART_TRANSMIT = 0,
    USART_BUFFER_OVERFLOW = 1,
    USART_WAITING = 2,
    USART_WRONG_LEN = 3,
} eUsartStatus;

extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_uart5_rx;
extern DMA_HandleTypeDef hdma_usart10_rx;
extern DMA_HandleTypeDef hdma_usart10_tx;

extern uint8_t uart_sts_rx_buffer[UART_STS_RX_BUFFER_SIZE];
extern uint8_t uart_sts_tx_buffer[UART_STS_TX_BUFFER_SIZE];

void Usartio_Init();
uint8_t usart_printf(const char *format, ...);
