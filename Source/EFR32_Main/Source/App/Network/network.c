/*
 * network.c
 *
 *  Created on: Apr 6, 2026
 *      Author: Nhan
 */

#include "app/framework/include/af.h"
#include "network.h"

EmberEventControl joinNetworkEventControl;
uint32_t timeFindAndJoin = 0;
networkEventHandler networkEventHandle = NULL;

/*
 * @function 			: Network_Init
 *
 * @brief				: Handle event network.
 *
 * @parameter			: networkEventHandler
 *
 * @return value		: None
 */
void Network_Init(networkEventHandler networkResult)
{
	networkEventHandle = networkResult;
}

/*
 * @function 			: NETWORK_FindAndJoin
 *
 * @brief				: Find network
 *
 * @parameter			: None
 *
 * @return value		: None
 */
void NETWORK_FindAndJoin(void)
{
	if(emberAfNetworkState() == EMBER_NO_NETWORK)
	{
		emberEventControlSetDelayMS(joinNetworkEventControl, 2000);
	}
}


/*
 * @function 			: NETWORK_StopFindAndJoin
 *
 * @brief				: Stop find network
 *
 * @parameter			: None
 *
 * @return value		: None
 */
void NETWORK_StopFindAndJoin(void)
{
	emberAfPluginNetworkSteeringStop();
}


/*
 * @function 			: joinNetworkEventHandler
 *
 * @brief				: Handle Event Join network
 *
 * @parameter			: None
 *
 * @return value		: None
 */
void joinNetworkEventHandler(void)
{
	emberEventControlSetInactive(joinNetworkEventControl);

	if(emberAfNetworkState() == EMBER_NO_NETWORK)
	{
		emberAfPluginNetworkSteeringStart();

		timeFindAndJoin++;

		emberEventControlSetDelayMS(joinNetworkEventControl, 2000);
	}
}

/*
 * @function 			: emberAfStackStatusCallback
 *
 * @brief				: Stack Status
 *
 * @parameter			: EmberStatus
 *
 * @return value		: True or false
 */
boolean emberAfStackStatusCallback(EmberStatus status)
{
	emberAfCorePrintln("emberAfStackStatusCallback\n");

	if(status == EMBER_NETWORK_UP)
	{

		if(timeFindAndJoin>0)
		{
			NETWORK_StopFindAndJoin();

			if(networkEventHandle != NULL)
			{
				emberAfCorePrintln("NETWORK_JOIN_SUCCESS");

				networkEventHandle(NETWORK_JOIN_SUCCESS);
			}
		}
		else
		{
			if(networkEventHandle != NULL)
			{
				emberAfCorePrintln("NETWORK_HAS_PARENT");

				networkEventHandle(NETWORK_HAS_PARENT);
			}
		}

	}
	else
	{
		EmberNetworkStatus nwkStatusCurrent = emberAfNetworkState();

		if(nwkStatusCurrent == EMBER_NO_NETWORK)
		{
			if(networkEventHandle != NULL)
			{
				networkEventHandle(NETWORK_OUT_NETWORK);
				emberAfCorePrintln("NETWORK_OUT_NETWORK");
			}
		}
		else if(nwkStatusCurrent == EMBER_JOINED_NETWORK_NO_PARENT){
			emberAfCorePrintln("NETWORK_LOST_PARENT");

			networkEventHandle(NETWORK_LOST_PARENT);
		}
	}

	return false;
}
