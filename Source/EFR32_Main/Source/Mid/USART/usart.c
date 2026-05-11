/*
 * usart.c
 *
 *  Created on: Apr 24, 2026
 *      Author: Nhan
 */


#include "app/framework/include/af.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "em_core.h"
#include "usart.h"

/* ============================================================
 * CẤU HÌNH CHÂN — thay đổi ở đây nếu cần chuyển chân
 * ============================================================ */
#define UART_TX_PORT        gpioPortC
#define UART_TX_PIN         0           /* PC00 */

#define UART_RX_PORT        gpioPortC
#define UART_RX_PIN         1           /* PC01 */

#define UART_BAUDRATE       115200

/* ============================================================
 * BIẾN NỘI BỘ
 * ============================================================ */



/* Callback người dùng đăng ký để nhận bản tin từ STM32 */
static UartComm_RxCallback_t s_rx_callback = NULL;



void UartComm_Init(UartComm_RxCallback_t cb)
{

    /* 1. Bật clock */
    CMU_ClockEnable(cmuClock_GPIO,   true);
    CMU_ClockEnable(cmuClock_USART2, true);

    /* 2. Cấu hình chân
     *    TX: push-pull, idle HIGH (mức idle của UART là 1)
     *    RX: input có pull-up để tránh nhiễu khi đối tác chưa init */
    GPIO_PinModeSet(UART_TX_PORT, UART_TX_PIN, gpioModePushPull,  1);
    GPIO_PinModeSet(UART_RX_PORT, UART_RX_PIN, gpioModeInputPull, 1);

    /* 3. Init USART ở chế độ async (UART) */
    USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;
    init.baudrate = UART_BAUDRATE;
    init.databits = usartDatabits8;
    init.parity   = usartNoParity;
    init.stopbits = usartStopbits1;
    init.enable   = usartDisable;   /* tạm disable để route xong mới enable */
    USART_InitAsync(USART2, &init);

    /* 4. Routing chân ra peripheral — EFR32xG21 dùng USARTROUTE[index]
     *    Chỉ số [2] tương ứng với USART2 */
    GPIO->USARTROUTE[2].TXROUTE =
          ((uint32_t)UART_TX_PORT << _GPIO_USART_TXROUTE_PORT_SHIFT)
        | ((uint32_t)UART_TX_PIN  << _GPIO_USART_TXROUTE_PIN_SHIFT);

    GPIO->USARTROUTE[2].RXROUTE =
          ((uint32_t)UART_RX_PORT << _GPIO_USART_RXROUTE_PORT_SHIFT)
        | ((uint32_t)UART_RX_PIN  << _GPIO_USART_RXROUTE_PIN_SHIFT);

    GPIO->USARTROUTE[2].ROUTEEN = GPIO_USART_ROUTEEN_TXPEN
                                | GPIO_USART_ROUTEEN_RXPEN;

    /* 5. Bật cả TX và RX */
    USART_Enable(USART2, usartEnable);
    s_rx_callback = cb;

    /* 7. Bật IRQ nhận (RXDATAV) và cấu hình NVIC
     *    Lưu ý: USART2_RX_IRQn — có hậu tố _RX cho EFR32 Series 2 */
    USART_IntClear(USART2, _USART_IF_MASK);       /* clear tất cả flag */
    USART_IntEnable(USART2, USART_IEN_RXDATAV);

    NVIC_ClearPendingIRQ(USART2_RX_IRQn);
    NVIC_EnableIRQ(USART2_RX_IRQn);

}

/* ============================================================
 * IRQ HANDLER — tên phải chính xác 'USART2_RX_IRQHandler'
 * để linker gắn vào vector table của EFR32MG21
 * ============================================================ */
void USART2_RX_IRQHandler(void)
{
    uint32_t flags = USART_IntGet(USART2);

    /* Log mọi flag để biết nguyên nhân IRQ */
    static uint32_t last_flags = 0;
    if (flags != last_flags) {
        emberAfCorePrintln("IRQ flags=0x%08X", flags);
        last_flags = flags;
    }

    if (flags & USART_IF_FERR) {
        emberAfCorePrintln("FERR (framing error)");
    }
    if (flags & USART_IF_RXOF) {
        emberAfCorePrintln("RXOF (overflow)");
    }

    if (flags & USART_IF_RXDATAV) {
        uint8_t count = USART_Rx(USART2);
        emberAfCorePrintln("RX: 0x%02X", count);

        s_rx_callback(count);
    }

    USART_IntClear(USART2, USART_IF_RXOF | USART_IF_FERR | USART_IF_PERR);
}
