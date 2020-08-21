/*
 * BLEDevice.h
 *
 *  Created on: Mar 22, 2017
 *      Author: kolban
 */

#ifndef MAIN_BLEDEVICE_H_
#define MAIN_BLEDEVICE_H_

#include <string.h>
#include <map>
#include <string>

#include "BLEAddress.h"
#include "BLEAdvertisedDevice.h"
#include "Seeed_erpcUnified.h"

class BLEClientCallbacks;
class BLEAdvertisedDevice;

/**
 * @brief A model of a %BLE client.
 */
class BLEClient {
public:
	BLEClient();
	bool 			 connect(BLEAdvertisedDevice* device);
	bool             connect(BLEAddress address, T_GAP_REMOTE_ADDR_TYPE type = GAP_REMOTE_ADDR_LE_PUBLIC);   // Connect to the remote BLE Server
	void             setClientCallbacks(BLEClientCallbacks *pClientCallbacks);
	

private:
    BLEClientCallbacks* m_pClientCallbacks;

}; // class BLEDevice


/**
 * @brief Callbacks associated with a %BLE client.
 */
class BLEClientCallbacks {
public:
	virtual ~BLEClientCallbacks() {};
	virtual void onConnect(BLEClient *pClient) = 0;
	virtual void onDisconnect(BLEClient *pClient) = 0;

};





#endif /* MAIN_BLEDEVICE_H_ */
