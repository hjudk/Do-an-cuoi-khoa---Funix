/*
 * receive.h
 *
 *  Created on: Apr 6, 2026
 *      Author: Nhan
 */

#ifndef SOURCE_APP_RECEIVE_RECEIVE_H_
#define SOURCE_APP_RECEIVE_RECEIVE_H_

#define LED_ON     1
#define LED_OFF    0

uint8_t checkBindingTable(uint8_t localEndpoint);
boolean emberAfPreCommandReceivedCallback(EmberAfClusterCommand* cmd);
bool RECEIVE_HandleLevelControlCluster(EmberAfClusterCommand* cmd);
bool RECEIVE_HandleOnOffCluster(EmberAfClusterCommand* cmd);

#endif /* SOURCE_APP_RECEIVE_RECEIVE_H_ */
