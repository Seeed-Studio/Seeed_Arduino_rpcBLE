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
		.mtu = 23
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
