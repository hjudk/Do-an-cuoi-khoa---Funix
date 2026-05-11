/*
 * main.c
 *
 *  Created on: Apr 6, 2026
 *      Author: Nhan
 */

#include "app/framework/include/af.h"


#include "Source/Mid/Led/led.h"
#include "Source/Mid/Button/button.h"
#include "protocol/zigbee/stack/include/binding-table.h"
#include "Source/App/Network/network.h"
#include "Source/App/Send/send.h"
#include "Source/App/Receive/receive.h"
#include "Source/Mid/PIR/pir.h"
#include "Source/Mid/USART/usart.h"
#include "Source/Mid/FIFO/fifo.h"


#include "main.h"
#include "math.h"

bool networkReady = false;
systemState system_State = POWER_ON_STATE;

void Main_ButtonPressCallbackHandler(uint8_t button, BUTTON_Event_t pressHandler);
void Main_ButtonHoldCallbackHandler(uint8_t button, BUTTON_Event_t holdingHandler);
void Main_networkEventHandler(uint8_t networkResult);
void Main_StmCountReceivedHandler(uint8_t count);

EmberEventControl mainStateEventControl;
EmberEventControl FindNetworkControl;
EmberEventControl MTORRsEventControl;
EmberEventControl mainUpdateCount;

Queue q;


void emberAfMainInitCallback(void)
{
	emberAfCorePrintln("Main Init"); // In log

	initQueue(&q);

	ledInit();

	buttonInit(Main_ButtonHoldCallbackHandler, Main_ButtonPressCallbackHandler);

	Network_Init(Main_networkEventHandler);

	UartComm_Init(Main_StmCountReceivedHandler);

	emberEventControlSetActive(mainStateEventControl);

	emberEventControlSetActive(mainUpdateCount);
}

static uint8_t count = 0;
static uint8_t prev_count = 0;

/*
 * @func	mainUpdateCountHandler
 * @brief	Event Update Count Handler
 * @param	None
 * @retval	None
 */
void mainUpdateCountHandler(void)
{
    emberEventControlSetInactive(mainUpdateCount);

    /* Process state machine: cặp 1→2 (vào), 2→1 (ra) */
    static uint8_t last_signal = 0;     /* 0 = chưa có signal */

    while (!isEmpty(q)) {
        uint8_t signal = deQueue(&q);

        if (signal == 0) {
            /* Reset count */
            count = 0;
            last_signal = 0;
            continue;
        }

        if (signal != 1 && signal != 2) {
            /* Bỏ qua giá trị không hợp lệ */
            continue;
        }

        if (last_signal == 0) {
            /* Chưa có signal trước, ghi nhớ */
            last_signal = signal;
        }
        else if (last_signal == 1 && signal == 2) {
            /* Cặp 1→2: người vào */
            count++;
            last_signal = 0;
            emberAfCorePrintln("Person ENTERED, count=%d", count);
        }
        else if (last_signal == 2 && signal == 1) {
            /* Cặp 2→1: người ra */
            if (count > 0) count--;
            last_signal = 0;
            emberAfCorePrintln("Person LEFT, count=%d", count);
        }
        else {
            /* Cùng loại signal (1→1 hoặc 2→2) — coi như reset, lấy cái mới */
            last_signal = signal;
        }
    }

    /* Update if count changed */
    if (count != prev_count) {
        USART_Tx(USART2, count);
        prev_count = count;

        if (count == 0) {
            SEND_OnOffStateReport(1, LED_OFF);
        } else {
            SEND_OnOffStateReport(1, LED_ON);
        }
    }

    /* Reschedule */
    emberEventControlSetDelayMS(mainUpdateCount, 2000);
}


static room_state_t s_room_state = STATE_UNKNOWN;

void Main_StmCountReceivedHandler(uint8_t count)
{
    emberAfCorePrintln("Count Received");

    room_state_t new_state = (count > 0) ? STATE_OCCUPIED : STATE_EMPTY;

    if (new_state != s_room_state) {
        if (new_state == STATE_OCCUPIED) {
        	SEND_OnOffStateReport(1, LED_ON);
        } else {
        	SEND_OnOffStateReport(1, LED_OFF);
        }
        s_room_state = new_state;
    }
}


/*
 * @func	Main_ButtonPressCallbackHandler
 * @brief	Event Button Handler
 * @param	button, pressHandler
 * @retval	None
 */
void Main_ButtonPressCallbackHandler(uint8_t button, BUTTON_Event_t pressHandler)
{
	switch(pressHandler)
	{
	case press_1:
		if(button == SW_2)
		{
			USART_Tx(USART2, 0xFF);
            emberAfCorePrintln("SW2: 1 time");
			toggleLed(LED1,ledPink,1,150,150);
		}
		break;

	case press_3:
		if(button == SW_1)
		{
            emberAfCorePrintln("SW2: 3 time");
			toggleLed(LED1,ledPink,1,150,150);
			emberAfPluginFindAndBindTargetStart(1);
		}
		else if(button == SW_2)
		{
            emberAfCorePrintln("SW2: 3 time");
			toggleLed(LED2,ledPink,1,150,150);
			emberAfPluginFindAndBindTargetStart(2);
		}
		break;

	case press_5:
		if(button == SW_2)
		{
			emberAfCorePrintln("SW2: 5 times");
			toggleLed(LED1,ledyellow, 2, 200, 200);
			system_State = REBOOT_STATE; // Chuyá»ƒn sang tráº¡ng thÃ¡i reboot
			emberEventControlSetDelayMS(mainStateEventControl,2000);
		}
		break;

	default:
		break;
	}
}


/*
 * @func	Main_ButtonHoldCallbackHandler
 * @brief	Event Button Handler
 * @param	button, holdingHandler
 * @retval	None
 */
void Main_ButtonHoldCallbackHandler(uint8_t button, BUTTON_Event_t holdingHandler)
{
	//
}

/*
 * @func	mainStateEventHandler
 * @brief	Handle Event State Network
 * @param	None
 * @retval	None
 */
void mainStateEventHandler(void)
{
	emberEventControlSetInactive(mainStateEventControl);

	EmberNetworkStatus nwkStatusCurrent;

	switch (system_State) {

	case POWER_ON_STATE:
		nwkStatusCurrent = emberAfNetworkState();
		if(nwkStatusCurrent == EMBER_NO_NETWORK)
		{
			toggleLed(LED1,ledRed,3,200,200);
			NETWORK_FindAndJoin();
		}
		system_State = IDLE_STATE;
		break;

	case REPORT_STATE:
		system_State = IDLE_STATE;
		SEND_ReportInfoHc();
		break;

	case IDLE_STATE:
		emberAfCorePrintln("IDLE_STATE");
		break;

	case REBOOT_STATE:
		system_State = IDLE_STATE;

		uint8_t contents[ZDO_MESSAGE_OVERHEAD + 1];
		contents[0] = 0x00;

		emberSendZigDevRequest(0x0000,
				LEAVE_RESPONSE,
				EMBER_AF_DEFAULT_APS_OPTIONS,
				contents,
				sizeof(contents));

		EmberNetworkStatus networkStatus = emberAfNetworkState();

		emberClearBindingTable();

		if (networkStatus == EMBER_JOINED_NETWORK)
		{
			emberLeaveNetwork();
		}
		halReboot(); // Reset MCU
		break;

	default:
		break;
	}
}

/*
 * @func	Main_networkEventHandler
 * @brief	Handler Event Result Network
 * @param	networkResult
 * @retval	None
 */
void Main_networkEventHandler(uint8_t networkResult)
{
	emberAfCorePrintln("Network Event Handle");

	switch (networkResult) {

	case NETWORK_HAS_PARENT:
		emberAfCorePrintln("Network has parent");
		toggleLed(LED1,ledPink,3,300,300);
		networkReady = true;
		system_State = REPORT_STATE;
		emberEventControlSetDelayMS(mainStateEventControl, 1000);
		break;

	case NETWORK_JOIN_FAIL:
		system_State = IDLE_STATE;
		toggleLed(LED1,ledBlue,3,300,300);
		emberAfCorePrintln("Network Join Fail");
		emberEventControlSetDelayMS(mainStateEventControl, 1000);
		break;

	case NETWORK_JOIN_SUCCESS:
		emberAfCorePrintln("Network Join Success");
		toggleLed(LED1,ledPink,3,300,300);
		networkReady =true;
		system_State = REPORT_STATE;
		emberEventControlSetDelayMS(mainStateEventControl, 1000);
		break;

	case NETWORK_LOST_PARENT:
		emberAfCorePrintln("Network lost parent");
		toggleLed(LED1,ledPink,3,300,300);
		system_State = REBOOT_STATE;
		emberEventControlSetDelayMS(mainStateEventControl, 1000);
		break;

	case NETWORK_OUT_NETWORK:
		if(networkReady)
		{
			toggleLed(LED1,ledRed,3,300,300);
			system_State = REBOOT_STATE;
			emberEventControlSetDelayMS(mainStateEventControl, 3000);
		}
	 	break;

	default:
		break;
	}
}

/*
 * @func	emberIncomingManyToOneRouteRequestHandler
 * @brief	Incoming MTORR Handler
 * @param	source, longID, cost
 * @retval	None
 */
void emberIncomingManyToOneRouteRequestHandler(EmberNodeId source,
		                                       EmberEUI64 longId,
											   uint8_t cost)
{
	// handle for MTORRS
	emberAfCorePrintln("Received MTORRS");
	emberEventControlSetInactive(MTORRsEventControl);
	emberEventControlSetDelayMS(MTORRsEventControl, 2 * ((uint8_t)halCommonGetRandom()));
}

/*
 * @func	MTORRsEventHandler
 * @brief	Read Status
 * @param	None
 * @retval	None
 */
void MTORRsEventHandler(void) {
	emberEventControlSetInactive(MTORRsEventControl);
	uint8_t data;

	EmberAfStatus status1 = emberAfReadServerAttribute(1,
												   ZCL_ON_OFF_CLUSTER_ID,
												   ZCL_ON_OFF_ATTRIBUTE_ID,
												   &data,
												   1);
	if(status1 == EMBER_ZCL_STATUS_SUCCESS)
	{
		SEND_OnOffStateReport(1,data);
	}

	EmberAfStatus status2 = emberAfReadServerAttribute(2,
												   ZCL_ON_OFF_CLUSTER_ID,
												   ZCL_ON_OFF_ATTRIBUTE_ID,
												   &data,
												   1);
	if(status2 == EMBER_ZCL_STATUS_SUCCESS)
	{
		SEND_OnOffStateReport(2,data);
	}
}
