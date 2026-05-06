/*
 * receive.c
 *
 *  Created on: Apr 6, 2026
 *      Author: Nhan
 */
#include "app/framework/include/af.h"
#include "Source/Mid/Led/led.h"
#include "Source/App/Send/send.h"
#include "Source/Mid/USART/usart.h"
#include "Source/Mid/FIFO/fifo.h"

#include "receive.h"


boolean emberAfPreCommandReceivedCallback(EmberAfClusterCommand *cmd)
{
	if(cmd->clusterSpecific)
	{
		switch(cmd->apsFrame->clusterId)
		{
		case ZCL_ON_OFF_CLUSTER_ID:
			return RECEIVE_HandleOnOffCluster(cmd);
		case ZCL_IAS_ZONE_CLUSTER_ID:
			return RECEIVE_HandleIASZoneCluster(cmd);

		default:
			break;
		}
    }

    return false;
}

boolean emberAfPreMessageReceivedCallback(EmberAfIncomingMessage* incommingMessage)
{

	if(incommingMessage->apsFrame->clusterId == ACTIVE_ENDPOINTS_RESPONSE)
	{
		return true;
	}

	return false;
}


uint8_t checkBindingTable(uint8_t localEndpoint)
{
	uint8_t index = 0;

	for(uint8_t i=0; i< EMBER_BINDING_TABLE_SIZE; i++)
	{
		EmberBindingTableEntry binding;

		if(emberGetBindingRemoteNodeId(i) != EMBER_SLEEPY_BROADCAST_ADDRESS)
		{
			if (emberGetBinding(i, &binding) == EMBER_SUCCESS)
			{
				if(binding.local == localEndpoint &&
				   binding.type == EMBER_UNICAST_BINDING)
				{
					index++;
				}
			}
		}
	}

	return index;
}
extern Queue q;
extern EmberEventControl mainUpdateCount;

bool RECEIVE_HandleIASZoneCluster(EmberAfClusterCommand* cmd)
{
	uint8_t localEndpoint = cmd ->apsFrame -> destinationEndpoint;

	if(localEndpoint == 1)
	{
		enQueue(&q,1);
		emberAfCorePrintln("Receive 1");
		toggleLed(LED1,ledBlue, 1, 200, 200);
	}
	else if (localEndpoint == 2)
	{
		enQueue(&q,2);
		emberAfCorePrintln("Receive 2");
		toggleLed(LED2,ledBlue, 1, 200, 200);

	}

	return false;
}

bool RECEIVE_HandleOnOffCluster(EmberAfClusterCommand* cmd)
{
	uint8_t commandID = cmd->commandId;
	uint8_t localEndpoint = cmd ->apsFrame -> destinationEndpoint;
	uint8_t remoteEndpoint = cmd->apsFrame -> sourceEndpoint;
	uint16_t IgnoreNodeID = cmd->source;

	emberAfCorePrintln("RECEIVE_HandleOnOffCluster SourceEndpoint = %d, RemoteEndpoint = %d, commandID = %d, nodeID %2X\n",
						remoteEndpoint,localEndpoint,commandID,IgnoreNodeID);

	switch(commandID)
	{
	case ZCL_OFF_COMMAND_ID:

		emberAfCorePrintln("LED OFF");

				if(localEndpoint == 1)
				{

					turnOffRBGLed(LED1);

					enQueue(&q,0);

					emberEventControlSetActive(mainUpdateCount);

				}

		break;


	case ZCL_ON_COMMAND_ID:
		emberAfCorePrintln("LED ON");

				if(localEndpoint == 1)
				{
					turnOnLed(LED1, ledRed);
				}

		break;

	default:
		break;
	}

	return false;
}
