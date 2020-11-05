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
#include "BLEFreeRTOS.h"
#include "BLEAddress.h"
#include "BLEAdvertisedDevice.h"
#include "BLERemoteService.h"
#include "BLERemoteDescriptor.h"
#include "seeed_rpcUnified.h"
#include "rtl_ble/ble_unified.h"

typedef uint8_t T_CLIENT_ID;
typedef uint8_t T_SERVER_ID; 
class BLERemoteService;
class BLEClientCallbacks;
class BLEAdvertisedDevice;
class BLERemoteCharacteristic;
class BLERemoteDescriptor;


/**
 * @brief A model of a %BLE client.
 */
class BLEClient {
public:
	BLEClient();
	~BLEClient();
	
	bool 			 connect(BLEAdvertisedDevice* device);
	bool             connect(BLEAddress address, T_GAP_REMOTE_ADDR_TYPE type = GAP_REMOTE_ADDR_LE_PUBLIC);   // Connect to the remote BLE Server
	void             disconnect();
	void             setClientCallbacks(BLEClientCallbacks *pClientCallbacks);
	BLEAddress       m_peerAddress = BLEAddress((uint8_t*)"\0\0\0\0\0\0");   // The BD address of the remote server.

	void                                       setValue(BLEUUID serviceUUID, BLEUUID characteristicUUID, std::string value);   // Set the value of a given characteristic at a given service.
	bool                                       isConnected();      
	uint8_t                                    getGattcIf();
	uint16_t                                   getConnId();
	std::map<std::string, BLERemoteService*>*  getServices();
	BLERemoteService*                          getService(const char* uuid);  // Get a reference to a specified service offered by the remote BLE server.
	BLERemoteService*                          getService(BLEUUID uuid);
	BLEAddress                                 getPeerAddress();              // Get the address of the remote BLE Server
	int                                        getRssi();                     // Get the RSSI of the remote BLE Server
    std::string                                getValue(BLEUUID serviceUUID, BLEUUID characteristicUUID);   // Get the value of a given characteristic at a given service.
	T_APP_RESULT                               handleGAPEvent(uint8_t cb_type, void *p_cb_data);
    std::string                                toString();                    // Return a string representation of this client.
	uint16_t								   getMTU();
	uint16_t			                       setMTU(uint16_t mtu_size);
	uint16_t         m_appId;
private:
    friend class BLEDevice;
	friend class BLERemoteService;
	friend class BLERemoteCharacteristic;
	friend class BLERemoteDescriptor;
	
	
	T_APP_RESULT   clientCallbackDefault(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data);	
	BLEClientCallbacks*   m_pClientCallbacks;
	uint16_t              m_conn_id;
	static uint8_t        m_gattc_if;
	bool                  m_haveServices = false;
    bool                  m_isConnected = false;	
	
	BLEFreeRTOS::Semaphore m_semaphoreSearchCmplEvt = BLEFreeRTOS::Semaphore("SearchCmplEvt");
	BLEFreeRTOS::Semaphore m_semaphoreRssiCmplEvt   = BLEFreeRTOS::Semaphore("RssiCmplEvt");
	void clearServices();   // Clear any existing services.
	std::map<std::string, BLERemoteService*> m_servicesMap;
	uint16_t m_mtu = 23;
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
