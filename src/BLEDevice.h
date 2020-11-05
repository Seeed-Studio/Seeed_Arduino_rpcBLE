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
#include "seeed_rpcUnified.h"
#include "rtl_ble/ble_unified.h"
typedef uint8_t T_CLIENT_ID;

class BLEDevice {
public:
    static BLEClient*  createClient();    // Create a new BLE client.
	static BLEServer*  createServer();    // Cretae a new BLE server.
    static BLEScan*    getScan();         // Get the scan object
	static BLEAddress  getAddress();      // Retrieve our own local BD address.
	static std::string getValue(BLEAddress bdAddress, BLEUUID serviceUUID, BLEUUID characteristicUUID);	  // Get the value of a characteristic of a service on a server.
	static void        setValue(BLEAddress bdAddress, BLEUUID serviceUUID, BLEUUID characteristicUUID, std::string value);   // Set the value of a characteristic on a service on a server.
	static void        init(std::string deviceName);   // Initialize the local BLE environment.
	static std::map<uint16_t, conn_status_t> getPeerDevices(bool client);
	static std::string toString();        // Return a string representation of our device.
	static void        whiteListAdd(T_GAP_WHITE_LIST_OP operation, uint8_t *bd_addr,T_GAP_REMOTE_ADDR_TYPE bd_type);   // Add an entry to the BLE white list.
	static void        whiteListRemove(T_GAP_WHITE_LIST_OP operation, uint8_t *bd_addr,T_GAP_REMOTE_ADDR_TYPE bd_type); // Remove an entry from the BLE white list.
	static void        addPeerDevice(void* peer, bool is_client, uint16_t conn_id);
	static void        removePeerDevice(uint16_t conn_id, bool client);
	static void        updatePeerDevice(void* peer, bool _client, uint16_t conn_id);
    static bool             getInitialized(); // Returns the state of the device, is it initialized or not?
	static BLEAdvertising* 	getAdvertising();
	static void		   		startAdvertising();
	static void		   		stopAdvertising();
	static BLEServer*       getServer();
	static BLEClient*       getClient();
	static void             setMTU(uint16_t mtu);
	static uint16_t	        getMTU();
	static BLEClient*       getClientByGattIf(uint16_t conn_id);
    static void             deinit();
    static uint16_t 	m_appId;
	static uint16_t		m_localMTU;
    static std::string  ble_name;
	static bool ble_start_flags;
private:
    friend class BLEClient;
	friend class BLEServer;
	static BLEServer*	m_pServer;
    static BLEClient*	m_pClient;
    static BLEScan*	   _pBLEScan;
	static BLEAdvertising* m_bleAdvertising;
	static std::map<uint16_t, conn_status_t> m_connectedClientsMap;
  
	static T_APP_RESULT gapEventHandler(uint8_t cb_type, void *p_cb_data);
	static T_APP_RESULT gattClientEventHandler(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data );
	static void         ble_handle_gap_msg(T_IO_MSG *p_gap_msg);
	static T_APP_RESULT gattServerEventHandler(T_SERVER_ID service_id, void *p_data);
};

#endif /* MAIN_BLEDevice_H_ */
