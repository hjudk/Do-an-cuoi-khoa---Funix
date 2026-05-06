/*
 * kalman.c
 *
 *  Created on: Mar 27, 2026
 *      Author: Nhan
 */
#include "app/framework/include/af.h"
#include "kalman.h"
#include <math.h>

void KalmanFilter_Init(KalmanFilter_t *kf, float q, float r, float initValue)
{
    kf->q = q;
    kf->r = r;
    kf->x = initValue;
    kf->p = 1.0f;
    kf->k = 0.0f;
}

float KalmanFilter_Update(KalmanFilter_t *kf, float measurement)
{
    /* Buoc 1: Du doan (Predict) */
    kf->p = kf->p + kf->q;

    /* Buoc 2: Tinh Kalman gain */
    kf->k = kf->p / (kf->p + kf->r);

    /* Buoc 3: Cap nhat uoc luong trang thai */
    kf->x = kf->x + kf->k * (measurement - kf->x);

    /* Buoc 4: Cap nhat sai so uoc luong */
    kf->p = (1.0f - kf->k) * kf->p;

    return kf->x;
}
