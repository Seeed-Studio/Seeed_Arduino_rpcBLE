/*
 * BLEServer.h
 *
 *  Created on: Apr 16, 2017
 *      Author: kolban
 */

#ifndef COMPONENTS_CPP_UTILS_BLESERVER_H_
#define COMPONENTS_CPP_UTILS_BLESERVER_H_


#include <string>
#include <string.h>

#include "BLEUUID.h"
#include "BLEAdvertising.h"
#include "BLECharacteristic.h"
#include "BLEService.h"
#include "BLEFreeRTOS.h"
#include "BLEAddress.h"
typedef uint8_t T_SERVER_ID; 

class BLEServerCallbacks;
/* TODO possibly refactor this struct */ 
typedef struct {
	void *peer_device;		// peer device BLEClient or BLEServer - maybe its better to have 2 structures or union here
	bool connected;			// do we need it?
	uint16_t mtu;			// every peer device negotiate own mtu
} conn_status_t;

/**
 * @brief A data structure that manages the %BLE servers owned by a BLE server.
 */
class BLEServiceMap {
public:	
    BLEService* getByUUID(const char* uuid);
	BLEService* getByUUID(BLEUUID uuid, uint8_t inst_id = 0);
    BLEService* getByHandle(uint16_t handle);
    void        setByUUID(BLEUUID uuid, BLEService* service);
    void        setByHandle(uint16_t handle, BLEService* service);
    void        setByUUID(const char* uuid, BLEService* service);
    BLEService* getFirst();
    BLEService* getNext();
    std::string toString();
    void        handleGATTServerEvent(T_SERVER_ID service_id, void *p_data);
    void 		removeService(BLEService *service);
    int 		getRegisteredServiceCount();
private:
    std::map<BLEService*, std::string> m_uuidMap;
    std::map<uint16_t, BLEService*>    m_handleMap;
    std::map<BLEService*, std::string>::iterator m_iterator;

};

/**
 * @brief The model of a %BLE server.
 */
class BLEServer {
public:
    BLEService*     createService(const char* uuid);
    BLEService*     createService(BLEUUID uuid, uint32_t numHandles=15, uint8_t inst_id=0);
    void            setCallbacks(BLEServerCallbacks* pCallbacks);
    BLEAdvertising* getAdvertising();
    void            startAdvertising();
    uint32_t        getConnectedCount();
    uint32_t        setConnectedCount();
    void 			removeService(BLEService* service);
    BLEService* 	getServiceByUUID(const char* uuid);
    BLEService* 	getServiceByUUID(BLEUUID uuid);
    void            addPeerDevice(void* peer, bool is_client, uint16_t conn_id);
    bool            removePeerDevice(uint16_t conn_id, bool client);
    uint16_t        getPeerMTU(uint16_t conn_id);
    bool 			connect(BLEAddress address);
    void 			disconnect(uint16_t connId);
    void			updateConnParams(uint8_t  conn_id,
                                     uint16_t  conn_interval_min,
                                     uint16_t  conn_interval_max,
                                     uint16_t  conn_latency,
                                     uint16_t  supervision_timeout,
                                     uint16_t  ce_length_min,
                                     uint16_t  ce_length_max);
    uint16_t        getconnId();
    BLEServerCallbacks* getCallbacks();
    std::map<uint16_t, conn_status_t> getPeerDevices(bool client);
    void updatePeerMTU(uint16_t connId, uint16_t mtu);
    uint16_t		m_appId;
private:
    BLEServer();
    friend class BLEDevice;

    uint16_t			m_connId;
    uint32_t            m_connectedCount;
    uint16_t            m_gatts_if;
    BLEServerCallbacks* m_pServerCallbacks = nullptr;
    std::map<uint16_t, conn_status_t> m_connectedServersMap;

    BLEFreeRTOS::Semaphore m_semaphoreRegisterAppEvt 	= BLEFreeRTOS::Semaphore("RegisterAppEvt");
    BLEFreeRTOS::Semaphore m_semaphoreCreateEvt 		= BLEFreeRTOS::Semaphore("CreateEvt");
    void            createApp(uint16_t appId);  
    BLEServiceMap       m_serviceMap;
	void             handleGATTServerEvent(T_SERVER_ID service_id, void *p_data);


}; // BLEServer


/**
 * @brief Callbacks associated with the operation of a %BLE server.
 */
class BLEServerCallbacks {
public:
	virtual ~BLEServerCallbacks() {};
	/**
	 * @brief Handle a new client connection.
	 *
	 * When a new client connects, we are invoked.
	 *
	 * @param [in] pServer A reference to the %BLE server that received the client connection.
	 */
	virtual void onConnect(BLEServer* pServer);
//	virtual void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t *param);
	/**
	 * @brief Handle an existing client disconnection.
	 *
	 * When an existing client disconnects, we are invoked.
	 *
	 * @param [in] pServer A reference to the %BLE server that received the existing client disconnection.
	 */
	virtual void onDisconnect(BLEServer* pServer);
}; // BLEServerCallbacks  

#endif /* COMPONENTS_CPP_UTILS_BLESERVER_H_ */
