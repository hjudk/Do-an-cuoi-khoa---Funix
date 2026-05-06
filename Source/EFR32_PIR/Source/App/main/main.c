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

#include "main.h"
#include "math.h"

bool networkReady = false;
systemState system_State = POWER_ON_STATE;

void Main_ButtonPressCallbackHandler(uint8_t button, BUTTON_Event_t pressHandler);
void Main_ButtonHoldCallbackHandler(uint8_t button, BUTTON_Event_t holdingHandler);
void Main_networkEventHandler(uint8_t networkResult);
void Main_PIREventHandler(uint8_t pirAction);


EmberEventControl mainStateEventControl;
EmberEventControl FindNetworkControl;
EmberEventControl MTORRsEventControl;




void emberAfMainInitCallback(void)
{
	emberAfCorePrintln("Main Init"); // In log

	ledInit();

	buttonInit(Main_ButtonHoldCallbackHandler, Main_ButtonPressCallbackHandler);

	Network_Init(Main_networkEventHandler);

	PIR_Init(Main_PIREventHandler);

	emberEventControlSetActive(mainStateEventControl);
}

void Main_PIREventHandler(uint8_t pirAction)
{
	switch (pirAction) {
		case PIR_MOTION:
				emberAfCorePrintln("PIR_MOTION");

				SEND_BindingInitToTarget(1,true);

				SEND_PIRStateReport(1,PIR_MOTION);


			toggleLed(LED1,ledBlue,1,150,150);
			turnOnLed(LED2,ledGreen);
			SEND_OnOffStateReport(2, LED_ON);

			break;
		case PIR_UNMOTION:
			turnOffRBGLed(LED2);
			SEND_OnOffStateReport(2, LED_OFF);

			emberAfCorePrintln("PIR_UNMOTION");

			SEND_PIRStateReport(1,PIR_UNMOTION);
			break;

		default:
			break;
	}
}


// Callback xá»­ lÃ½ khi nháº¥n nÃºt
void Main_ButtonPressCallbackHandler(uint8_t button, BUTTON_Event_t pressHandler)
{
	switch(pressHandler)
	{
	case press_1: // Nháº¥n 1 láº§n
		if(button == SW_2)
		{
			toggleLed(LED1,ledPink,1,150,150);
		}
		break;

	case press_4: // Nháº¥n 1 láº§n
		if(button == SW_2)
		{
			toggleLed(LED1,ledPink,1,150,150);
			emberAfPluginFindAndBindInitiatorStart(1);
		}
		break;
	case press_5: // Nháº¥n 5 láº§n
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


// Callback giá»¯ nÃºt (chÆ°a xá»­ lÃ½)
void Main_ButtonHoldCallbackHandler(uint8_t button, BUTTON_Event_t holdingHandler)
{
	//
}


// Event handler tráº¡ng thÃ¡i chÃ­nh
void mainStateEventHandler(void)
{
	emberEventControlSetInactive(mainStateEventControl); // Táº¯t event sau khi xá»­ lÃ½

	EmberNetworkStatus nwkStatusCurrent;

	switch (system_State) {

	case POWER_ON_STATE: // Khi vá»«a báº­t nguá»“n
		nwkStatusCurrent = emberAfNetworkState();
		if(nwkStatusCurrent == EMBER_NO_NETWORK)
		{
			toggleLed(LED1,ledRed,3,200,200);
			NETWORK_FindAndJoin(); // TÃ¬m vÃ  join máº¡ng
		}
		system_State = IDLE_STATE;
		break;

	case REPORT_STATE: // Gá»­i dá»¯ liá»‡u
		system_State = IDLE_STATE;
		SEND_ReportInfoHc();
		break;

	case IDLE_STATE: // Tráº¡ng thÃ¡i chá»�
		emberAfCorePrintln("IDLE_STATE");
		break;

	case REBOOT_STATE: // Reset thiáº¿t bá»‹
		system_State = IDLE_STATE;

		uint8_t contents[ZDO_MESSAGE_OVERHEAD + 1];
		contents[0] = 0x00;

		// Gá»­i lá»‡nh leave tá»›i coordinator
		emberSendZigDevRequest(0x0000,
				LEAVE_RESPONSE,
				EMBER_AF_DEFAULT_APS_OPTIONS,
				contents,
				sizeof(contents));

		EmberNetworkStatus networkStatus = emberAfNetworkState();

		emberClearBindingTable(); // XÃ³a báº£ng binding

		if (networkStatus == EMBER_JOINED_NETWORK)
		{
			emberLeaveNetwork(); // Rá»�i máº¡ng
		}
		halReboot(); // Reset MCU
		break;

	default:
		break;
	}
}


// Xá»­ lÃ½ sá»± kiá»‡n network
void Main_networkEventHandler(uint8_t networkResult)
{
	emberAfCorePrintln("Network Event Handle");

	switch (networkResult) {

	case NETWORK_HAS_PARENT: // Ä�Ã£ cÃ³ parent
		emberAfCorePrintln("Network has parent");
		toggleLed(LED1,ledPink,3,300,300);
		networkReady = true;
		system_State = REPORT_STATE;
		emberEventControlSetDelayMS(mainStateEventControl, 1000);
		break;

	case NETWORK_JOIN_FAIL: // Join tháº¥t báº¡i
		system_State = IDLE_STATE;
		toggleLed(LED1,ledBlue,3,300,300);
		emberAfCorePrintln("Network Join Fail");
		emberEventControlSetDelayMS(mainStateEventControl, 1000);
		break;

	case NETWORK_JOIN_SUCCESS: // Join thÃ nh cÃ´ng
		emberAfCorePrintln("Network Join Success");
		toggleLed(LED1,ledPink,3,300,300);
		networkReady =true;
		system_State = REPORT_STATE;
		emberEventControlSetDelayMS(mainStateEventControl, 1000);
		break;

	case NETWORK_LOST_PARENT: // Máº¥t káº¿t ná»‘i parent
		emberAfCorePrintln("Network lost parent");
		toggleLed(LED1,ledPink,3,300,300);
		system_State = REBOOT_STATE;
		emberEventControlSetDelayMS(mainStateEventControl, 1000);
		break;

	case NETWORK_OUT_NETWORK: // Bá»‹ out khá»�i máº¡ng
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


void emberIncomingManyToOneRouteRequestHandler(EmberNodeId source,
		                                       EmberEUI64 longId,
											   uint8_t cost)
{
	// handle for MTORRS
	emberAfCorePrintln("Received MTORRS");
	emberEventControlSetInactive(MTORRsEventControl);
	emberEventControlSetDelayMS(MTORRsEventControl, 2 * ((uint8_t)halCommonGetRandom()));
}

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
