/*
 * BLERemoteCharacteristic.cpp
 *
 *  Created on: Jul 8, 2017
 *      Author: kolban
 */

#include "BLERemoteCharacteristic.h"
#include <sstream>

BLERemoteCharacteristic *BLERemoteCharacteristic::_this = NULL;

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
	m_end_handle      = value_handle;
	m_pRemoteService = pRemoteService;
//	m_notifyCallback = nullptr;
	m_rawData = nullptr;

	retrieveDescriptors(); // Get the descriptors for this characteristic
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
	return m_handle;
} // getHandle

uint16_t BLERemoteCharacteristic::getendHandle() {
	return m_end_handle;
} // getHandle


/**
 * @brief Get the remote service associated with this characteristic.
 * @return The remote service associated with this characteristic.
 */
BLERemoteService* BLERemoteCharacteristic::getRemoteService() {
	return m_pRemoteService;
} // getRemoteService

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

/**
 * @brief Populate the descriptors (if any) for this characteristic.
 */
void BLERemoteCharacteristic::retrieveDescriptors() {

	removeDescriptors();   // Remove any existing descriptors.
	client_all_char_descriptor_discovery(getRemoteService()->getClient()->getConnId(),getRemoteService()->getClient()->getGattcIf(),
                                                m_handle,m_end_handle);

	
    BLERemoteCharacteristic::_this = this;
    m_semaphoregetdescEvt.take("getDescriptor");
	m_haveDescriptor = (m_semaphoregetdescEvt.wait("getDescriptor") == 0);
} // getDescriptors

/**
 * @brief Delete the descriptors in the descriptor map.
 * We maintain a map called m_descriptorMap that contains pointers to BLERemoteDescriptors
 * object references.  Since we allocated these in this class, we are also responsible for deleteing
 * them.  This method does just that.
 * @return N/A.
 */
void BLERemoteCharacteristic::removeDescriptors() {
	// Iterate through all the descriptors releasing their storage and erasing them from the map.
	for (auto &myPair : getRemoteService()->getClient()->m_descriptorMap) {
	   getRemoteService()->getClient()->m_descriptorMap.erase(myPair.first);
	   delete myPair.second;
	}
	getRemoteService()->getClient()->m_descriptorMap.clear();   // Technically not neeeded, but just to be sure.
} // removeCharacteristics



