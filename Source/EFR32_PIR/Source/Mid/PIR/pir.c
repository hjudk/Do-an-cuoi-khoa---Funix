/*
 * pir.c
 *
 *  Created on: Apr 8, 2026
 *      Author: Nhan
 */


#include "app/framework/include/af.h"
#include "pir.h"
#include "gpiointerrupt.h"
#include "stddef.h"
#include "em_cmu.h"
#include "em_gpio.h"

pirControl PirCallbackFunc = NULL;
pirActionHandler_e pirState;
EmberEventControl pirDetectEventControl;
boolean status=false;

void PIR_INTSignalHandle(uint8_t pin);
boolean isMotionSignal(void);

/**
 * @func    PIR_Init
 * @brief   Initialize PIR
 * @param   PirHandler
 * @retval  None
 */
void PIR_Init(pirControl PirHandler)
{
	CMU_ClockEnable(cmuClock_GPIO, true);
	GPIOINT_Init();
	GPIO_PinModeSet(PIR_PORT,
					PIR_PIN,
					gpioModeInput,
					0);
	/* Register callbacks before setting up and enabling pin interrupt */
	GPIOINT_CallbackRegister(PIR_PIN,
							 PIR_INTSignalHandle);

	PIR_Enable(true);
	PirCallbackFunc = PirHandler;

}

/**
 * @func    PIR_Enable
 * @brief   Enable PIR
 * @param   enable
 * @retval  None
 */
void PIR_Enable(boolean enable)
{
	if(enable)
	{
		GPIO_ExtIntConfig(PIR_PORT,PIR_PIN,PIR_PIN,
						  true,false, true);
	}
	else
	{
		GPIO_ExtIntConfig(PIR_PORT,PIR_PIN,PIR_PIN,
						  false,false,false);
	}
}

/**
 * @func    PIR_INTSignalHandle
 * @brief   Event PIR Handler
 * @param   pin
 * @retval  None
 */
void PIR_INTSignalHandle(uint8_t pin)
{
	status=true;
	if(pin != PIR_PIN)
	{
		return;
	}
	if(isMotionSignal())
	{

		pirState = PIR_STATE_DEBOUNCE;
		PIR_Enable(false);

		emberEventControlSetInactive(pirDetectEventControl);
		emberEventControlSetDelayMS(pirDetectEventControl, 100);
	}
}

/**
 * @func    pirDetectEventHandler
 * @brief   Event PIR state Handler
 * @param   None
 * @retval  None
 */
void pirDetectEventHandler(void)
{
	emberEventControlSetInactive(pirDetectEventControl);

	switch (pirState) {
		case PIR_STATE_DEBOUNCE:
			if(isMotionSignal())
			{
				emberAfCorePrintln("PIR_DETECT_MOTION");
				pirState = PIR_STATE_WAIT_2_5S;
				if(PirCallbackFunc!= NULL)
				{
					PirCallbackFunc(PIR_MOTION);
				}
				emberEventControlSetDelayMS(pirDetectEventControl, 2500);
			}else
			{
				PIR_Enable(true);
			}
			break;
		case PIR_STATE_WAIT_2_5S:
			{
				status = false;
				pirState = PIR_STATE_WAIT_15S;
				PIR_Enable(true);
				emberEventControlSetDelayMS(pirDetectEventControl, 15000);
			}
		break;
		case PIR_STATE_WAIT_15S:
			{
				if(PirCallbackFunc != NULL)
				{
					emberAfCorePrintln("PIR_DETECT_UNMOTION");
					PirCallbackFunc(PIR_UNMOTION);
				}
			}
			break;
		default:
			break;
	}
}


boolean isMotionSignal(void)
{
	boolean isMotion;
	if(status == true)
	{
		isMotion = true;
	}
	else
	{
		isMotion = false;
	}
	return isMotion;
}

