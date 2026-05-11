/*
 * pir.h
 *
 *  Created on: Apr 8, 2026
 *      Author: Nhan
 */

#ifndef SOURCE_MID_PIR_PIR_H_
#define SOURCE_MID_PIR_PIR_H_

#define PIR_PORT						gpioPortC
#define PIR_PIN							(4U)

typedef enum{
	PIR_UNMOTION,
	PIR_MOTION,
	PIR_STATE_DEBOUNCE,
	PIR_STATE_WAIT_2_5S,
	PIR_STATE_WAIT_15S

}pirActionHandler_e;

typedef void(*pirControl) (pirActionHandler_e PirEvent);

void PIR_Init(pirControl PirHandler);
void PIR_Enable(boolean enable);

#endif /* SOURCE_MID_PIR_PIR_H_ */
