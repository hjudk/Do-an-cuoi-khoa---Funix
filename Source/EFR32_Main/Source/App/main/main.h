/*
 * main.h
 *
 *  Created on: Apr 6, 2026
 *      Author: Nhan
 */

#ifndef SOURCE_APP_MAIN_MAIN_H_
#define SOURCE_APP_MAIN_MAIN_H_

typedef enum{
	POWER_ON_STATE,
	REPORT_STATE,
	IDLE_STATE,
	REBOOT_STATE
}systemState;

typedef enum {
	STATE_UNKNOWN,
	STATE_EMPTY,
	STATE_OCCUPIED
} room_state_t;

#endif /* SOURCE_APP_MAIN_MAIN_H_ */
