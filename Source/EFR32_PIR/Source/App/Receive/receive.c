/*
 * receive.c
 *
 *  Created on: Apr 6, 2026
 *      Author: Nhan
 */
#include "app/framework/include/af.h"
#include "Source/Mid/Led/led.h"
#include "Source/App/Send/send.h"
#include "receive.h"

/*
void USART2_RX_IRQHandler(void)
{
        uint8_t count = (uint8_t)USART_Rx(USART2);

        if (count > 0)
        {
        	SEND_OnOffStateReport(1, LED_ON);
        }
        else
        {
        	SEND_OnOffStateReport(1, LED_OFF);
        }


}
*/
boolean emberAfPreCommandReceivedCallback(EmberAfClusterCommand *cmd)
{

	if(cmd->clusterSpecific)
	{
		switch(cmd->apsFrame->clusterId)
		{
		case ZCL_ON_OFF_CLUSTER_ID:
			return RECEIVE_HandleOnOffCluster(cmd);

		default:
			break;
		}
    }

    // TrÃ¡ÂºÂ£ false Ã¢â€ â€™ cho phÃƒÂ©p framework xÃ¡Â»Â­ lÃƒÂ½ tiÃ¡ÂºÂ¿p nÃ¡ÂºÂ¿u chÃ†Â°a xÃ¡Â»Â­ lÃƒÂ½
    return false;
}



uint8_t checkBindingTable(uint8_t localEndpoint)
{
	uint8_t index = 0; // BiÃ¡ÂºÂ¿n Ã„â€˜Ã¡ÂºÂ¿m sÃ¡Â»â€˜ binding hÃ¡Â»Â£p lÃ¡Â»â€¡

	// DuyÃ¡Â»â€¡t toÃƒÂ n bÃ¡Â»â„¢ bÃ¡ÂºÂ£ng binding
	for(uint8_t i=0; i< EMBER_BINDING_TABLE_SIZE; i++)
	{
		EmberBindingTableEntry binding;

		// KiÃ¡Â»Æ’m tra entry cÃƒÂ³ tÃ¡Â»â€œn tÃ¡ÂºÂ¡i (khÃƒÂ´ng phÃ¡ÂºÂ£i Ã„â€˜Ã¡Â»â€¹a chÃ¡Â»â€° broadcast)
		if(emberGetBindingRemoteNodeId(i) != EMBER_SLEEPY_BROADCAST_ADDRESS)
		{
			// LÃ¡ÂºÂ¥y thÃƒÂ´ng tin binding
			if (emberGetBinding(i, &binding) == EMBER_SUCCESS)
			{
				// KiÃ¡Â»Æ’m tra Ã„â€˜ÃƒÂºng endpoint vÃƒÂ  kiÃ¡Â»Æ’u unicast binding
				if(binding.local == localEndpoint &&
				   binding.type == EMBER_UNICAST_BINDING)
				{
					index++; // TÃ„Æ’ng sÃ¡Â»â€˜ lÃ†Â°Ã¡Â»Â£ng binding hÃ¡Â»Â£p lÃ¡Â»â€¡
				}
			}
		}
	}

	return index; // TrÃ¡ÂºÂ£ vÃ¡Â»ï¿½ sÃ¡Â»â€˜ binding
}


bool RECEIVE_HandleOnOffCluster(EmberAfClusterCommand* cmd)
{
	uint8_t commandID = cmd->commandId; // Lấy ID của command (ON/OFF)
	uint8_t localEndpoint = cmd ->apsFrame -> destinationEndpoint; // Endpoint của thiết bị nhận
	uint8_t remoteEndpoint = cmd->apsFrame -> sourceEndpoint; // Endpoint của thiết bị gửi
	uint16_t IgnoreNodeID = cmd->source; // NodeID của thiết bị gửi

	// In log thông tin nhận được
	emberAfCorePrintln("RECEIVE_HandleOnOffCluster SourceEndpoint = %d, RemoteEndpoint = %d, commandID = %d, nodeID %2X\n",
						remoteEndpoint,localEndpoint,commandID,IgnoreNodeID);

	switch(commandID)
	{
	case ZCL_OFF_COMMAND_ID: // Lệnh OFF

		emberAfCorePrintln("LED OFF");

		switch (cmd->type) {

			case EMBER_INCOMING_UNICAST: // Nhận unicast
				emberAfCorePrintln("Unicast recieved");
					// Tắt LED1
					turnOffRBGLed(LED2);

					// Gửi report trạng thái OFF
					SEND_OnOffStateReport(localEndpoint, LED_OFF);

				break;

			// ----------- MULTICAST -----------
			case EMBER_INCOMING_MULTICAST:
				emberAfCorePrintln("Multicast recieved");
					turnOffRBGLed(LED2);
					SEND_OnOffStateReport(localEndpoint, LED_OFF);

				break;

			default:
				break;
		}

		break;


	case ZCL_ON_COMMAND_ID: // Lệnh ON

		emberAfCorePrintln("LED ON");

		emberAfCorePrintln("Turn on LED");

		switch (cmd->type)
		{
			case EMBER_INCOMING_UNICAST: // Nhận unicast
					// Bật LED2
					turnOnLed(LED2, ledGreen);

					// Gửi report
					SEND_OnOffStateReport(localEndpoint, LED_ON);

				break;

			case EMBER_INCOMING_MULTICAST: // Nhận multicast
				emberAfCorePrintln("Multicast");
				turnOnLed(LED2, ledGreen);
					SEND_OnOffStateReport(localEndpoint, LED_ON);


				break;

			default:
				break;
		}
		break;

	default:
		break;
	}

	// Trả false → cho phép framework xử lý tiếp nếu cần
	return false;
}
