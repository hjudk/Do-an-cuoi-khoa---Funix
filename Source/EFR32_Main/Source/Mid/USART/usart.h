/*
 * usart.h
 *
 *  Created on: Apr 24, 2026
 *      Author: Nhan
 */

#ifndef SOURCE_MID_USART_USART_H_
#define SOURCE_MID_USART_USART_H_

#include <stdint.h>

/* ============================================================
 * CẤU HÌNH CHÂN — thay đổi ở đây nếu cần chuyển chân
 * ============================================================ */
#define UART_TX_PORT        gpioPortC
#define UART_TX_PIN         0           /* PC00 */

#define UART_RX_PORT        gpioPortC
#define UART_RX_PIN         1           /* PC01 */

#define UART_BAUDRATE       115200


typedef void (*UartComm_RxCallback_t)(uint8_t count);



/* Khởi tạo USART2, chân PC00/PC01, baudrate 115200, bật IRQ nhận.
 * Phải gọi trong emberAfMainInitCallback() TRƯỚC mọi cuộc gọi gửi.
 * Truyền cb = NULL nếu không cần xử lý bản tin đến. */
void UartComm_Init(UartComm_RxCallback_t cb);



#endif /* SOURCE_MID_USART_USART_H_ */
