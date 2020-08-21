/*
 * BLEDevice.cpp
 *
 *  Created on: Mar 22, 2017
 *      Author: kolban
 */
#include "BLEClient.h"

#include <string>
#include <sstream>
#include <unordered_set>
#include "BLEDevice.h"
#include "BLEAdvertisedDevice.h"

/*
 * Design
 * ------
 * When we perform a searchService() requests, we are asking the BLE server to return each of the services
 * that it exposes.  For each service, we received an ESP_GATTC_SEARCH_RES_EVT event which contains details
 * of the exposed service including its UUID.
 *
 * The objects we will invent for a BLEClient will be as follows:
 * * BLERemoteService - A model of a remote service.
 * * BLERemoteCharacteristic - A model of a remote characteristic
 * * BLERemoteDescriptor - A model of a remote descriptor.
 *
 * Since there is a hierarchical relationship here, we will have the idea that from a BLERemoteService will own
 * zero or more remote characteristics and a BLERemoteCharacteristic will own zero or more remote BLEDescriptors.
 *
 * We will assume that a BLERemoteService contains a map that maps BLEUUIDs to the set of owned characteristics
 * and that a BLECharacteristic contains a map that maps BLEUUIDs to the set of owned descriptors.
 *
 *
 */

BLEClient::BLEClient() {
} // BLEClient

/**
 * @brief Set the callbacks that will be invoked.
 */
void BLEClient::setClientCallbacks(BLEClientCallbacks* pClientCallbacks) {
	m_pClientCallbacks = pClientCallbacks;
} // setClientCallbacks


/**
 * Add overloaded function to ease connect to peer device with not public address
 */
bool BLEClient::connect(BLEAdvertisedDevice* device) {
	BLEAddress address =  device->getAddress();
	T_GAP_REMOTE_ADDR_TYPE type = device->getAddressType();
	return connect(address, type);
}


/**
 * @brief Connect to the partner (BLE Server).
 * @param [in] address The address of the partner.
 * @return True on success.
 */
bool BLEClient::connect(BLEAddress address, T_GAP_REMOTE_ADDR_TYPE type) {
//连接到client
#if 0
	le_set_conn_param(GAP_CONN_PARAM_1M, &_connReqParam);
    T_GAP_CAUSE result = le_connect(0, destAddr.data(), destAddrType, _localAddrType, scanTimeout);

    if (result == GAP_CAUSE_SUCCESS) {
        if (BTDEBUG) printf("Connect successful to %s\r\n", destAddr.str());
        return true;
    } else {
        if (BTDEBUG) printf("Connect failed\r\n");
        return false;
    }
#endif 	
} // connect

