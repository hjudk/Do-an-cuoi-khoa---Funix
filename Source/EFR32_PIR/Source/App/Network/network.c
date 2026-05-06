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


void Network_Init(networkEventHandler networkResult)
{
	// Lưu lại con trỏ hàm callback để xử lý các sự kiện mạng sau này
	networkEventHandle = networkResult;
}


void NETWORK_FindAndJoin(void)
{
	// Kiểm tra nếu thiết bị chưa tham gia mạng nào
	if(emberAfNetworkState() == EMBER_NO_NETWORK)
	{
		// Đặt event sau 2 giây để bắt đầu quá trình tìm và join mạng
		emberEventControlSetDelayMS(joinNetworkEventControl, 2000);
	}
}



void NETWORK_StopFindAndJoin(void)
{
	// Dừng quá trình network steering (tìm và tham gia mạng)
	emberAfPluginNetworkSteeringStop();
}



void joinNetworkEventHandler(void)
{
	// Tắt event hiện tại (event trong Ember là one-shot)
	emberEventControlSetInactive(joinNetworkEventControl);

	// Nếu vẫn chưa vào mạng
	if(emberAfNetworkState() == EMBER_NO_NETWORK)
	{
		// Bắt đầu quá trình tìm và join mạng (network steering)
		emberAfPluginNetworkSteeringStart();

		// Tăng biến đếm số lần thử join mạng
		timeFindAndJoin++;

		// Đặt lại event sau 2 giây để tiếp tục thử lại nếu chưa thành công
		emberEventControlSetDelayMS(joinNetworkEventControl, 2000);
	}
}


boolean emberAfStackStatusCallback(EmberStatus status)
{
	// In log mỗi khi có thay đổi trạng thái stack Zigbee
	emberAfCorePrintln("emberAfStackStatusCallback\n");

	// Nếu mạng đã hoạt động (join thành công hoặc có parent)
	if(status == EMBER_NETWORK_UP)
	{
		// Nếu đã từng thực hiện tìm mạng (timeFindAndJoin > 0)
		// => Đây là trường hợp join mạng thành công
		if(timeFindAndJoin>0)
		{
			// Dừng quá trình tìm mạng
			NETWORK_StopFindAndJoin();

			// Nếu có đăng ký callback
			if(networkEventHandle != NULL)
			{
				emberAfCorePrintln("NETWORK_JOIN_SUCCESS");

				// Gọi callback báo join thành công
				networkEventHandle(NETWORK_JOIN_SUCCESS);
			}
		}
		else
		{
			// Trường hợp thiết bị đã có parent sẵn (ví dụ reboot nhưng vẫn còn network)
			if(networkEventHandle != NULL)
			{
				emberAfCorePrintln("NETWORK_HAS_PARENT");

				// Gọi callback báo đã có parent
				networkEventHandle(NETWORK_HAS_PARENT);
			}
		}

	}
	else //Nhận bản tin leave từ HC
	{
		// Lấy trạng thái mạng hiện tại
		EmberNetworkStatus nwkStatusCurrent = emberAfNetworkState();

		// Nếu hoàn toàn không còn trong mạng
		if(nwkStatusCurrent == EMBER_NO_NETWORK)
		{
			if(networkEventHandle != NULL)
			{
				// Báo sự kiện ra khỏi mạng
				networkEventHandle(NETWORK_OUT_NETWORK);
				emberAfCorePrintln("NETWORK_OUT_NETWORK");
			}
		}
		// Nếu vẫn trong mạng nhưng mất parent (mất kết nối)
		else if(nwkStatusCurrent == EMBER_JOINED_NETWORK_NO_PARENT){
			emberAfCorePrintln("NETWORK_LOST_PARENT");

			// Báo sự kiện mất parent
			networkEventHandle(NETWORK_LOST_PARENT);
		}
	}

	// Trả về false để cho stack tiếp tục xử lý mặc định
	return false;
}
