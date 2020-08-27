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
#include "Seeed_erpcUnified.h"


typedef uint8_t T_CLIENT_ID;
typedef uint8_t T_SERVER_ID; 
class BLERemoteService;
class BLEClientCallbacks;
class BLEAdvertisedDevice;
class BLERemoteCharacteristic;


/**
 * @brief A model of a %BLE client.
 */
class BLEClient {
public:
	BLEClient();
	~BLEClient();
	
	bool 			 connect(BLEAdvertisedDevice* device);
	bool             connect(BLEAddress address, T_GAP_REMOTE_ADDR_TYPE type = GAP_REMOTE_ADDR_LE_PUBLIC);   // Connect to the remote BLE Server
	void                                       disconnect();
	void             setClientCallbacks(BLEClientCallbacks *pClientCallbacks);
	BLEAddress       m_peerAddress = BLEAddress((uint8_t*)"\0\0\0\0\0\0");   // The BD address of the remote server.
	
	uint8_t                                    getGattcIf();
	uint16_t                                   getConnId();
	std::map<std::string, BLERemoteService*>*  getServices(); 
	BLERemoteService*                          getService(BLEUUID uuid);  
	
	std::map<std::string, BLERemoteCharacteristic*> m_characteristicMap;
	std::map<uint16_t, BLERemoteCharacteristic*> m_characteristicMapByHandle;
	
//	static BLEClient*     _this;
	
	uint16_t         m_appId;
private:
    friend class BLEDevice;
	friend class BLERemoteService;
	friend class BLERemoteCharacteristic;
	
	
	T_APP_RESULT   clientCallbackDefault(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data);
    
//	static T_CLIENT_ID    _basClientId;
	
	BLEClientCallbacks*   m_pClientCallbacks;
	uint16_t              m_conn_id;
	static uint8_t        m_gattc_if;
	bool                  m_haveServices = false;
    bool                  m_isConnected = false;	
	
	BLEFreeRTOS::Semaphore m_semaphoreSearchCmplEvt = BLEFreeRTOS::Semaphore("SearchCmplEvt");
	void clearServices();   // Clear any existing services.
	std::map<std::string, BLERemoteService*> m_servicesMap;
	
	//std::map<std::string, BLERemoteCharacteristic*> m_characteristicMap;
	//std::map<uint16_t, BLERemoteCharacteristic*> m_characteristicMapByHandle;
	

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
