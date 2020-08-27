/*
 * BLERemoteCharacteristic.cpp
 *
 *  Created on: Jul 8, 2017
 *      Author: kolban
 */

#include "BLERemoteCharacteristic.h"
#include <sstream>


BLERemoteCharacteristic::BLERemoteCharacteristic(
    uint16_t    decl_handle,  
    uint16_t    properties,   
    uint16_t    value_handle,  
    uint16_t    uuid16,
	BLERemoteService*    pRemoteService
    ) {
	m_handle         = decl_handle;
	m_srvcId16       = uuid16;
	m_uuid           = BLEUUID(m_srvcId16);
	m_charProp       = value_handle;
	m_pRemoteService = pRemoteService;
//	m_notifyCallback = nullptr;
	m_rawData = nullptr;

//	retrieveDescriptors16(); // Get the descriptors for this characteristic
} // BLERemoteCharacteristic

/**
 * @brief Get the UUID for this characteristic.
 * @return The UUID for this characteristic.
 */
BLEUUID BLERemoteCharacteristic::getUUID() {
	return m_uuid;
} // getUUID

/**
 * @brief Get the handle for this characteristic.
 * @return The handle for this characteristic.
 */
uint16_t BLERemoteCharacteristic::getHandle() {
	//log_v(">> getHandle: Characteristic: %s", getUUID().toString().c_str());
	//log_v("<< getHandle: %d 0x%.2x", m_handle, m_handle);
	return m_handle;
} // getHandle

/**
 * @brief Convert a BLERemoteCharacteristic to a string representation;
 * @return a String representation.
 */
std::string BLERemoteCharacteristic::toString() {
	std::string res = "Characteristic: uuid: " + m_uuid.toString();
	char val[6];
	res += ", handle: ";
	snprintf(val, sizeof(val), "%d", getHandle());
	res += val;
	res += " 0x";
	snprintf(val, sizeof(val), "%04x", getHandle());
	res += val;
//	res += ", props: " + BLEUtils::characteristicPropertiesToString(m_charProp);
	return res;
} // toString




