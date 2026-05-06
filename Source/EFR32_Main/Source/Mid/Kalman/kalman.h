/*
 * kalman.h
 *
 *  Created on: Mar 27, 2026
 *      Author: Nhan
 */

#ifndef SOURCE_MIDDLE_KALMAN_KALMAN_H_
#define SOURCE_MIDDLE_KALMAN_KALMAN_H_

typedef struct {
    float q;    // Nhiễu hệ thống
    float r;    // Nhiễu đo
    float x;    // Giá trị ước lượng
    float p;    // Sai số ước lượng
    float k;    // Kalman gain
} KalmanFilter_t;

void KalmanFilter_Init(KalmanFilter_t *kf, float q, float r, float initValue);
float KalmanFilter_Update(KalmanFilter_t *kf, float measurement);

#endif /* SOURCE_MIDDLE_KALMAN_KALMAN_H_ */
