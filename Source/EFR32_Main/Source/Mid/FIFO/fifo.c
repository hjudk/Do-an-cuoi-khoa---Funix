/*
 * fifo.c
 *
 *  Created on: Apr 25, 2026
 *      Author: Nhan
 */

#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "fifo.h"

/**
 * @func   initQueue
 * @brief  FIFO queue initialize
 * @param  q
 * @retval None
 */
void initQueue(Queue* q) {
    q->front = 0;
    q->rear = -1;
}

/**
 * @func   initQueue
 * @brief  FIFO queue check empty
 * @param  q
 * @retval True or false
 */
int isEmpty(Queue q) {
    return q.front > q.rear;
}

/**
 * @func   isFull
 * @brief  FIFO queue check full
 * @param  q
 * @retval True or false
 */
int isFull(Queue q) {
    if ((q.rear - q.front + 1) == FIFO_SIZE) {
        return 1;
    }
    else {
        return 0;
    }
}

/**
 * @func   enQueue
 * @brief  FIFO enqueue
 * @param  1, x
 * @retval None
 */
void enQueue(Queue *q, uint8_t x) {
    if (!isFull(*q)) {
        if (q->rear == -1) { // Hàng đợi rỗng
            q->front = 0;
            q->rear = 0;
        } else if (q->rear == FIFO_SIZE - 1) { // Đầy ở cuối -> dồn về đầu
            for (int i = q->front; i <= q->rear; i++) {
                q->data[i - q->front] = q->data[i];
            }
            q->rear = q->rear - q->front;
            q->front = 0;
            q->rear++;
        } else {  // Bình thường, chỉ tăng rear
            q->rear++;
        }
        q->data[q->rear] = x;
    }
}

/**
 * @func   enQueue
 * @brief  FIFO dequeue
 * @param  q
 * @retval d
 */
uint8_t deQueue(Queue* q) {
	uint8_t d= 0;
    if (!isEmpty(*q)) {
        d = q->data[q->front];
        q->front++;
    }
    if (q->front > q->rear) { // Nếu hàng đợi rỗng sau khi lấy -> khởi tạo lại
        initQueue(q);
    }
    return d;
}
