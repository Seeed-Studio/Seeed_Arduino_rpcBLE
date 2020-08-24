/*
 * BLEDevice.h
 *
 *  Created on: Mar 16, 2017
 *      Author: kolban
 */

#ifndef MAIN_BLEDevice_H_
#define MAIN_BLEDevice_H_

#include <map>               // Part of C++ STL
#include <string>
#include "BLEServer.h"
#include "BLEScan.h"
#include "FreeRTOS.h"
#include "BLEClient.h"
#define BTDEBUG 1
typedef uint8_t T_CLIENT_ID;

class BLEDevice {
public:
    static BLEClient*  createClient();    // Create a new BLE client.
    static BLEScan*    getScan();         // Get the scan object
	static void        init(std::string deviceName);   // Initialize the local BLE environment.
	static std::map<uint16_t, conn_status_t> getPeerDevices(bool client);
	static uint16_t 	m_appId;
	static void addPeerDevice(void* peer, bool is_client, uint16_t conn_id);
private:
    static BLEClient*	m_pClient;
    static BLEScan*	   _pBLEScan;
	static std::map<uint16_t, conn_status_t> m_connectedClientsMap;
	static T_APP_RESULT gapEventHandler(uint8_t cb_type, void *p_cb_data);
    static void gattClientEventHandler(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data );	
	
};	

#endif /* MAIN_BLEDevice_H_ */
