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

/**
 * @func    SEND_SendCommandUnicast
 * @brief   Send uinicast command
 * @param   source, destination, address
 * @retval  None
 */
static void SEND_SendCommandUnicast(uint8_t source,
							 uint8_t destination,
							 uint8_t address)
{
	emberAfSetCommandEndpoints(source, destination);

	(void) emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, address);
}


/**
 * @func    SEND_FillBufferGlobalCommand
 * @brief   Send fill buffer global command
 * @param   clusterID, attributeID, globalCommand, value, length, dataType
 * @retval  None
 */
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

	data[2] = EMBER_SUCCESS;

	data[3] = (uint8_t)dataType;

	memcpy(&data[4], value, length);

	// Fill buffer ZCL
	(void) emberAfFillExternalBuffer(
			(ZCL_GLOBAL_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
			clusterID,
			globalCommand,
			"b",
			data,
			length + 4
	);
}

/**
 * @func    SEND_ReportInfoHc
 * @brief   Send Report to HC
 * @param   None
 * @retval  None
 */
void SEND_ReportInfoHc(void)
{
	// Model ID
	uint8_t modelID[13] = {9, 'P','I','R','1','_','S','W','1'};

	// Manufacturer name
	uint8_t manufactureID[5] = {4, 'L', 'u', 'm', 'i'};

	// Version
	uint8_t version = 1;

	if(emberAfNetworkState() != EMBER_JOINED_NETWORK){
		return;
	}

	SEND_FillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,
								 ZCL_MODEL_IDENTIFIER_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 modelID,
								 13,
								 ZCL_CHAR_STRING_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(SOURCE_ENDPOINT_PRIMARY,
							DESTINATTION_ENDPOINT,
							HC_NETWORK_ADDRESS);


	SEND_FillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,
								 ZCL_MANUFACTURER_NAME_ATTRIBUTE_ID,
								 ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								 manufactureID,
								 5,
								 ZCL_CHAR_STRING_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(SOURCE_ENDPOINT_PRIMARY,
							DESTINATTION_ENDPOINT,
							HC_NETWORK_ADDRESS);


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


/**
 * @func    SEND_BindingInitToTarget
 * @brief   Send Binding command
 * @param   remoteEndpoint, localEndpoint, value, nodeID
 * @retval  None
 */
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


/**
 * @func    SEND_OnOffStateReport
 * @brief   Send On/Off State
 * @param   Endpoint, value
 * @retval  None
 */
void SEND_OnOffStateReport(uint8_t Endpoint, uint8_t value){

	SEND_FillBufferGlobalCommand(ZCL_ON_OFF_CLUSTER_ID,
						   ZCL_ON_OFF_ATTRIBUTE_ID,
						   ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
						   (uint8_t*) &value,
						   1,
						   ZCL_BOOLEAN_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(Endpoint,
								DESTINATTION_ENDPOINT,
								HC_NETWORK_ADDRESS);

	emberAfWriteServerAttribute(Endpoint,
								ZCL_ON_OFF_CLUSTER_ID,
								ZCL_ON_OFF_ATTRIBUTE_ID,
								(uint8_t*) &value,
								ZCL_BOOLEAN_ATTRIBUTE_TYPE);
}

/**
 * @func    SEND_PIRStateReport
 * @brief   Send PIR value to App
 * @param   Endpoint, value
 * @retval  None
 */
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
