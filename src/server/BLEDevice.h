/*
 * BLEDevice.h
 *
 *  Created on: Mar 16, 2020  
 *      Author: coolc
 */

#ifndef MAIN_BLEDevice_H_
#define MAIN_BLEDevice_H_

#include <map>               // Part of C++ STL
#include <string>
#include "BLEServer.h"
#include "BLEScan.h"
#include "BLEFreeRTOS.h"
#include "BLEClient.h"
#include "BLEAdvertising.h"
#include "Seeed_erpcUnified.h"
#define BTDEBUG 0
typedef uint8_t T_CLIENT_ID;

class BLEDevice {
public:
    static BLEClient*  createClient();    // Create a new BLE client.
	static BLEServer*  createServer();    // Cretae a new BLE server.
    static BLEScan*    getScan();         // Get the scan object
	static void        init(std::string deviceName);   // Initialize the local BLE environment.
	static std::map<uint16_t, conn_status_t> getPeerDevices(bool client);
	static uint16_t 	m_appId;
	static void addPeerDevice(void* peer, bool is_client, uint16_t conn_id);
	static void removePeerDevice(uint16_t conn_id, bool client);

	static BLEAdvertising* 	getAdvertising();
	static void		   		startAdvertising();

private:
    friend class BLEClient;
	static BLEServer*	m_pServer;
    static BLEClient*	m_pClient;
    static BLEScan*	   _pBLEScan;
	static BLEAdvertising* m_bleAdvertising;
	static std::map<uint16_t, conn_status_t> m_connectedClientsMap;
  
	static T_APP_RESULT gapEventHandler(uint8_t cb_type, void *p_cb_data);
	static T_APP_RESULT gattClientEventHandler(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data );
	static void ble_handle_gap_msg(T_IO_MSG *p_gap_msg);



//*********************ble server eventhandler*****************************************************
	static void gattServerEventHandler(T_SERVER_ID service_id, void *p_datas);
};

#endif /* MAIN_BLEDevice_H_ */
