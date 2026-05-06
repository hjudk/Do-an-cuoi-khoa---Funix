/*
 * send.c
 *
 *  Created on: Apr 6, 2026
 *      Author: Nhan
 */

#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "app/framework/include/af.h"
#include "send.h"
#include "Source/App/Receive/receive.h"
#include "zigbee-framework/zigbee-device-common.h"

// Send UNICAST
static void SEND_SendCommandUnicast(uint8_t source,
							 uint8_t destination,
							 uint8_t address)
{
	// Thiáº¿t láº­p endpoint nguá»“n vÃ  endpoint Ä‘Ã­ch cho frame ZCL
	emberAfSetCommandEndpoints(source, destination);

	// Gá»­i command theo kiá»ƒu unicast trá»±c tiáº¿p tá»›i nodeID (address)
	(void) emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, address);
}



static void SEND_FillBufferGlobalCommand(EmberAfClusterId clusterID,
								  EmberAfAttributeId attributeID,
								  uint8_t globalCommand,
								  uint8_t* value,
								  uint8_t length,
								  uint8_t dataType)
{
	uint8_t data[MAX_DATA_COMMAND_SIZE];

	// Byte 0-1: Attribute ID (Little Endian)
	data[0] = (uint8_t)(attributeID & 0x00FF);
	data[1] = (uint8_t)((attributeID & 0xFF00)>>8);

	// Byte 2: Status (EMBER_SUCCESS = Ä‘á»�c thÃ nh cÃ´ng)
	data[2] = EMBER_SUCCESS;

	// Byte 3: Kiá»ƒu dá»¯ liá»‡u cá»§a attribute
	data[3] = (uint8_t)dataType;

	// Byte 4 trá»Ÿ Ä‘i: giÃ¡ trá»‹ attribute
	memcpy(&data[4], value, length);

	// Fill buffer ZCL (global command - vÃ­ dá»¥ Read Attribute Response)
	(void) emberAfFillExternalBuffer(
			(ZCL_GLOBAL_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
			clusterID,
			globalCommand,
			"b",        // kiá»ƒu dá»¯ liá»‡u buffer (byte array)
			data,
			length + 4  // tá»•ng Ä‘á»™ dÃ i payload
	);
}


// Gá»­i thÃ´ng tin thiáº¿t bá»‹ lÃªn Home Controller (HC)
void SEND_ReportInfoHc(void)
{
	// Model ID (chuá»—i Zigbee dáº¡ng length + string)
	uint8_t modelID[13] = {9, 'P','I','R','1','_','S','W','1'};

	// Manufacturer name
	uint8_t manufactureID[5] = {4, 'L', 'u', 'm', 'i'};

	// Version á»©ng dá»¥ng
	uint8_t version = 1;

	// Náº¿u chÆ°a join máº¡ng â†’ khÃ´ng gá»­i
	if(emberAfNetworkState() != EMBER_JOINED_NETWORK){
		return;
	}

	// ---- Gá»­i Model ID ----
	SEND_FillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,
								 ZCL_MODEL_IDENTIFIER_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 modelID,
								 13,
								 ZCL_CHAR_STRING_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(SOURCE_ENDPOINT_PRIMARY,
							DESTINATTION_ENDPOINT,
							HC_NETWORK_ADDRESS);


	// ---- Gá»­i Manufacturer ----
	SEND_FillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,
								 ZCL_MANUFACTURER_NAME_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 manufactureID,
								 5,
								 ZCL_CHAR_STRING_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(SOURCE_ENDPOINT_PRIMARY,
							DESTINATTION_ENDPOINT,
							HC_NETWORK_ADDRESS);


	// ---- Gá»­i Version ----
	SEND_FillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,
								 ZCL_APPLICATION_VERSION_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 &version,
								 1,
								 ZCL_INT8U_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(SOURCE_ENDPOINT_PRIMARY,
							DESTINATTION_ENDPOINT,
							HC_NETWORK_ADDRESS);
}



void SEND_BindingInitToTarget(uint8_t localEndpoint, boolean value)
{
	EmberStatus status = EMBER_INVALID_BINDING_INDEX;

	uint8_t indexNodeID = checkBindingTable(localEndpoint)-1;

	uint16_t currentTargetNodeID = emberGetBindingRemoteNodeId(indexNodeID);

	// In debug
	emberAfCorePrintln("index: 0x%2X",indexNodeID);
	emberAfCorePrintln("node id: 0x%2X",currentTargetNodeID);

	EmberBindingTableEntry binding;

	for(uint8_t i = 0; i< EMBER_BINDING_TABLE_SIZE ; i++)
	{
		status = emberGetBinding(i, &binding);

		if(status != EMBER_SUCCESS)
		{
			return;
		}
	}

	emberGetBinding(indexNodeID, &binding);

	emberAfCorePrintln("local endpoint: 0x%X",binding.local);
	emberAfCorePrintln("remote endpoint: 0x%X",binding.remote);

	if((currentTargetNodeID != EMBER_SLEEPY_BROADCAST_ADDRESS) &&
	   (currentTargetNodeID != EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS) &&
	   (currentTargetNodeID != EMBER_BROADCAST_ADDRESS))
	{
			switch (value) {

				case true:
					emberAfCorePrintln("SEND ON INIT TO TARGET");

				    emberAfFillCommandIasZoneClusterZoneStatusChangeNotification(
				            1, 0, 0, 0);

					emberAfSetCommandEndpoints(binding.local, binding.remote);

					emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, currentTargetNodeID);
					break;

				case false:
					emberAfCorePrintln("SEND OFF INIT TO TARGET");

				    emberAfFillCommandIasZoneClusterZoneStatusChangeNotification(
				            0, 0, 0, 0);

					emberAfSetCommandEndpoints(binding.local, binding.remote);

					emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, currentTargetNodeID);
					break;
			}
	}
}



void SEND_OnOffStateReport(uint8_t Endpoint, uint8_t value){

	// Táº¡o gÃ³i tin report tráº¡ng thÃ¡i ON/OFF
	SEND_FillBufferGlobalCommand(ZCL_ON_OFF_CLUSTER_ID,
						   ZCL_ON_OFF_ATTRIBUTE_ID,
						   ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
						   (uint8_t*) &value,
						   1,
						   ZCL_BOOLEAN_ATTRIBUTE_TYPE);

	// Gá»­i vá»� Home Controller
	SEND_SendCommandUnicast(Endpoint,
								DESTINATTION_ENDPOINT,
								HC_NETWORK_ADDRESS);

	// Cáº­p nháº­t attribute trong local database (ZCL server)
	emberAfWriteServerAttribute(Endpoint,
								ZCL_ON_OFF_CLUSTER_ID,
								ZCL_ON_OFF_ATTRIBUTE_ID,
								(uint8_t*) &value,
								ZCL_BOOLEAN_ATTRIBUTE_TYPE);
}

void SEND_PIRStateReport(uint8_t endpoint, uint8_t value){
	SEND_FillBufferGlobalCommand(ZCL_IAS_ZONE_CLUSTER_ID,
								 ZCL_ZONE_STATUS_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 (uint8_t*) &value,
								 1,
						   	   	 ZCL_BOOLEAN_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(endpoint,
								DESTINATTION_ENDPOINT,
								HC_NETWORK_ADDRESS);

	emberAfWriteServerAttribute(endpoint,
								ZCL_IAS_ZONE_CLUSTER_ID,
								ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								(uint8_t*) &value,
								ZCL_BOOLEAN_ATTRIBUTE_TYPE);
}
