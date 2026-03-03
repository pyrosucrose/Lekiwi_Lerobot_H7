//
// Created by Glucose_carbide on 25-8-8.
//

#ifndef USARTIO_HPP
#define USARTIO_HPP

// =============================== 调用库 ===============================
#include "usart.h"

// =============================== 宏定义区 ===============================
inline constexpr uint16_t UART1_RX_BUFFER_SIZE = 255;
inline constexpr uint16_t UART1_TX_BUFFER_SIZE = 255;
inline constexpr uint16_t UART5_RX_BUFFER_SIZE = 255;
inline constexpr uint16_t UART5_TX_BUFFER_SIZE = 255;
inline constexpr uint16_t UART10_RX_BUFFER_SIZE = 255;
inline constexpr uint16_t UART10_TX_BUFFER_SIZE = 255;

// =============================== 变量区 ===============================
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_uart5_rx;
extern DMA_HandleTypeDef hdma_usart10_rx;
extern DMA_HandleTypeDef hdma_usart10_tx;

extern uint8_t uart1_rx_buffer[UART1_RX_BUFFER_SIZE];
extern uint8_t uart1_tx_buffer[UART1_TX_BUFFER_SIZE];
extern uint8_t uart10_rx_buffer[UART10_RX_BUFFER_SIZE];
extern uint8_t uart10_tx_buffer[UART10_TX_BUFFER_SIZE];

// =============================== 函数声明 ===============================
void Usartio_Init();
void usart_printf(const char *format, ...);

// =============================== 类声明 ===============================

#endif // USARTIO_HPP
