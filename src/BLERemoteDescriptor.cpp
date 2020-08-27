/*
 * BLERemoteDescriptor.cpp
 *
 *  Created on: Jul 8, 2017
 *      Author: kolban
 */

#include <sstream>
#include "BLERemoteDescriptor.h"

BLERemoteDescriptor::BLERemoteDescriptor(
	uint16_t                 handle,
	BLEUUID                  uuid,
	BLERemoteCharacteristic* pRemoteCharacteristic) {

	m_handle                = handle;
	m_uuid                  = uuid;
	m_pRemoteCharacteristic = pRemoteCharacteristic;
}


/**
 * @brief Return a string representation of this BLE Remote Descriptor.
 * @retun A string representation of this BLE Remote Descriptor.
 */
std::string BLERemoteDescriptor::toString() {
	char val[6];
	snprintf(val, sizeof(val), "%d", getHandle());
	std::string res = "handle: ";
	res += val;
	res += ", uuid: " + getUUID().toString();
	return res;
} // toString

/**
 * @brief Retrieve the UUID associated this remote descriptor.
 * @return The UUID associated this remote descriptor.
 */
BLEUUID BLERemoteDescriptor::getUUID() {
	return m_uuid;
} // getUUID

/**
 * @brief Retrieve the handle associated with this remote descriptor.
 * @return The handle associated with this remote descriptor.
 */
uint16_t BLERemoteDescriptor::getHandle() {
	return m_handle;
} // getHandle




