/*
 * BLEServer.cpp
 *
 *  Created on: Apr 16, 2017
 *      Author: kolban
 */
#define TAG "BLEServer"
#include "BLEDevice.h"
#include "BLEServer.h"
//#include "BLEService.h"
#include <string.h>
#include <string>
#include "rpc_unified_log.h"
#include <unordered_set>

/**
 * @brief Construct a %BLE Server
 *
 * This class is not designed to be individually instantiated.  Instead one should create a server by asking
 * the BLEDevice class.
 */
BLEServer::BLEServer() {
	m_appId            = 0xff;
	m_gatts_if         = 0xff;
	m_connectedCount   = 0;
	m_connId           = 0xff;
	m_pServerCallbacks = nullptr;
} // BLEServer


void BLEServer::createApp(uint16_t appId) {
	m_appId = appId;
} // createApp


/**
 * @brief Create a %BLE Service.
 *
 * With a %BLE server, we can host one or more services.  Invoking this function causes the creation of a definition
 * of a new service.  Every service must have a unique UUID.
 * @param [in] uuid The UUID of the new service.
 * @return A reference to the new service object.
 */
BLEService* BLEServer::createService(const char* uuid) {
	return createService(BLEUUID(uuid));
}


/**
 * @brief Create a %BLE Service.
 *
 * With a %BLE server, we can host one or more services.  Invoking this function causes the creation of a definition
 * of a new service.  Every service must have a unique UUID.
 * @param [in] uuid The UUID of the new service.
 * @param [in] numHandles The maximum number of handles associated with this service.
 * @param [in] inst_id With multiple services with the same UUID we need to provide inst_id value different for each service.
 * @return A reference to the new service object.
 */
BLEService* BLEServer::createService(BLEUUID uuid, uint32_t numHandles, uint8_t inst_id) {
	BLEService* pService = new BLEService(uuid, numHandles);
	pService->m_instId = inst_id;
	m_serviceMap.setByUUID(uuid, pService); // Save a reference to this service being on this server.
	pService->executeCreate(this);          // Perform the API calls to actually create the service.
	return pService;
} // createService


/**
 * @brief Set the server callbacks.
 *
 * As a %BLE server operates, it will generate server level events such as a new client connecting or a previous client
 * disconnecting.  This function can be called to register a callback handler that will be invoked when these
 * events are detected.
 *
 * @param [in] pCallbacks The callbacks to be invoked.
 */
void BLEServer::setCallbacks(BLEServerCallbacks* pCallbacks) {
	m_pServerCallbacks = pCallbacks;
} // setCallbacks

BLEServerCallbacks* BLEServer::getCallbacks() {
	return m_pServerCallbacks;
}

void BLEServerCallbacks::onConnect(BLEServer* pServer) {

} // onConnect

void BLEServerCallbacks::onDisconnect(BLEServer* pServer) {

} // onDisconnect

void BLEServer::addPeerDevice(void* peer, bool _client, uint16_t conn_id) {
	conn_status_t status = {
		.peer_device = peer,
		.connected = true,
		.mtu = 247
	};
    m_connId = conn_id;
	m_connectedServersMap.insert(std::pair<uint16_t, conn_status_t>(conn_id, status));	
}

bool BLEServer::removePeerDevice(uint16_t conn_id, bool _client) {
	return m_connectedServersMap.erase(conn_id) > 0;
}
/* multi connect support */

uint16_t BLEServer::getPeerMTU(uint16_t conn_id) {
	return m_connectedServersMap.find(conn_id)->second.mtu;
}

uint16_t  BLEServer::getconnId(){
	return m_connId;
}

/* multi connect support */
/* TODO do some more tweaks */
void BLEServer::updatePeerMTU(uint16_t conn_id, uint16_t mtu) {
	// set mtu in conn_status_t
	const std::map<uint16_t, conn_status_t>::iterator it = m_connectedServersMap.find(conn_id);
	if (it != m_connectedServersMap.end()) {
		it->second.mtu = mtu;
		std::swap(m_connectedServersMap[conn_id], it->second);
	}
}

/**
 * @brief Return the number of connected clients.
 * @return The number of connected clients.
 */
uint32_t BLEServer::getConnectedCount() {
	return m_connectedCount;
} // getConnectedCount

uint32_t BLEServer::setConnectedCount() {
	return m_connectedCount++;
} // getConnectedCount


/**
 * @brief Get a %BLE Service by its UUID
 * @param [in] uuid The UUID of the new service.
 * @return A reference to the service object.
 */
BLEService* BLEServer::getServiceByUUID(const char* uuid) {
	return m_serviceMap.getByUUID(BLEUUID(uuid));
}

/**
 * @brief Get a %BLE Service by its UUID
 * @param [in] uuid The UUID of the new service.
 * @return A reference to the service object.
 */
BLEService* BLEServer::getServiceByUUID(BLEUUID uuid) {
	return m_serviceMap.getByUUID(uuid);
}

/**
 * Allow to connect GATT server to peer device
 * Probably can be used in ANCS for iPhone
 */
bool BLEServer::connect(BLEAddress address) {

} // connect

void BLEServer::disconnect(uint16_t connId) {

}

/**
 * Update connection parameters can be called only after connection has been established
 */
void BLEServer::updateConnParams(uint8_t  conn_id,
                                 uint16_t  conn_interval_min,
                                 uint16_t  conn_interval_max,
                                 uint16_t  conn_latency,
                                 uint16_t  supervision_timeout,
                                 uint16_t  ce_length_min,
                                 uint16_t  ce_length_max) {
	    uint8_t  update_conn_id = conn_id;
        uint16_t update_conn_interval_min = conn_interval_min;
        uint16_t update_conn_interval_max = conn_interval_max;
        uint16_t update_conn_latency = conn_latency;
        uint16_t update_supervision_timeout = supervision_timeout;
        uint16_t update_ce_length_min = ce_length_min;
        uint16_t update_ce_length_max = ce_length_max;
		le_update_conn_param(update_conn_id,
                            update_conn_interval_min,
                            update_conn_interval_max,
                            update_conn_latency,
                            update_supervision_timeout,
                            update_ce_length_min,
                            update_ce_length_max
                            );
}

/**
 * @brief Retrieve the advertising object that can be used to advertise the existence of the server.
 *
 * @return An advertising object.
 */
BLEAdvertising* BLEServer::getAdvertising() {
	return BLEDevice::getAdvertising();
}

/**
 * @brief Start advertising.
 *
 * Start the server advertising its existence.  This is a convenience function and is equivalent to
 * retrieving the advertising object and invoking start upon it.
 */
void BLEServer::startAdvertising() {
	BLEDevice::startAdvertising();
} // startAdvertising



std::map<uint16_t, conn_status_t> BLEServer::getPeerDevices(bool _client) {
	return m_connectedServersMap;
}

/*
 * Remove service
 */
void BLEServer::removeService(BLEService* service) {
	service->stop();
	service->executeDelete(service->getgiff());	
	m_serviceMap.removeService(service);
}


/**
 * @brief Handle a GATT Server Event.
 *
 * @param [in] event
 * @param [in] gatts_if
 * @param [in] param
 *
 */
void BLEServer::handleGATTServerEvent(T_SERVER_ID service_id, void *p_data) {
    RPC_DEBUG("into server :: handleGATTServerEvent\n\r");
	// Invoke the handler for every Service we have.
	m_serviceMap.handleGATTServerEvent(service_id,p_data);
} // handleGATTServerEvent
