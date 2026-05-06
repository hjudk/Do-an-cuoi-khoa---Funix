/*
 * fifo.h
 *
 *  Created on: Apr 25, 2026
 *      Author: Nhan
 */

#ifndef SOURCE_MID_FIFO_FIFO_H_
#define SOURCE_MID_FIFO_FIFO_H_

#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define FIFO_SIZE              16

typedef struct {
    uint8_t data[FIFO_SIZE]; // mảng lưu dữ liệu các điểm
    int front; // chỉ số đầu hàng
    int rear; // chỉ số cuối hàng
} Queue;


// Khởi tạo hàng đợi rỗng
void initQueue(Queue* q);

// Kiểm tra hàng đợi có rỗng không
int isEmpty(Queue q);

// Kiểm tra hàng đợi có đầy không
int isFull(Queue q);

// Hàm thêm phần tử vào cuối hàng đợi (enqueue)
void enQueue(Queue *q, uint8_t x);

// Hàm lấy phần tử ra khỏi đầu hàng đợi (dequeue)
uint8_t deQueue(Queue* q);

#endif /* SOURCE_MID_FIFO_FIFO_H_ */
