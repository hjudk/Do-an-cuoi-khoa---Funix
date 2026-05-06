/*
 * send.h
 *
 *  Created on: Apr 6, 2026
 *      Author: Nhan
 */

#ifndef SOURCE_APP_SEND_SEND_H_
#define SOURCE_APP_SEND_SEND_H_

#define MAX_DATA_COMMAND_SIZE					50
#define SOURCE_ENDPOINT_PRIMARY					1
#define SOURCE_ENDPOINT_SECONDARY				2
#define DESTINATTION_ENDPOINT					1
#define HC_NETWORK_ADDRESS						0x0000
 #define ZDO_MESSAGE_OVERHEAD 					1

static void SEND_SendCommandUnicast(uint8_t source,
							 uint8_t destination,
							 uint8_t address);
static void SEND_FillBufferGlobalCommand(EmberAfClusterId clusterID,
								  EmberAfAttributeId attributeID,
								  uint8_t globalCommand,
								  uint8_t* value,
								  uint8_t length,
								  uint8_t dataType);

void SEND_OnOffStateReport(uint8_t Endpoint, uint8_t value);
void SEND_PIRStateReport(uint8_t Endpoint, uint8_t value);
void SEND_ReportInfoHc(void);

void SEND_BindingInitToTarget(uint8_t localEndpoint, boolean value);
//void SEND_BindingInitToTarget(uint8_t remoteEndpoint, uint8_t localEndpoint, bool value, uint16_t nodeID);

void SEND_LDRStateReport(uint8_t Endpoint, uint32_t value);
#endif /* SOURCE_APP_SEND_SEND_H_ */
