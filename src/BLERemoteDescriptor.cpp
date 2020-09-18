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

/**
 * @brief Get the characteristic that owns this descriptor.
 * @return The characteristic that owns this descriptor.
 */
BLERemoteCharacteristic* BLERemoteDescriptor::getRemoteCharacteristic() {
	return m_pRemoteCharacteristic;
} // getRemoteCharacteristic


void BLERemoteDescriptor::writeValue(uint8_t* data, size_t length, bool response) {
	// Check to see that we are connected.
	if (!getRemoteCharacteristic()->getRemoteService()->getClient()->isConnected()) {
		return;
	}
	client_attr_write(m_pRemoteCharacteristic->getRemoteService()->getClient()->getConnId(),m_pRemoteCharacteristic->getRemoteService()->getClient()->getGattcIf(),GATT_WRITE_TYPE_REQ,getHandle(),length,(uint8_t *)data);
} // writeValue


/**
 * @brief Write data represented as a string to the BLE Remote Descriptor.
 * @param [in] newValue The data to send to the remote descriptor.
 * @param [in] response True if we expect a response.
 */
void BLERemoteDescriptor::writeValue(std::string newValue, bool response) {
	writeValue((uint8_t*) newValue.data(), newValue.length(), response);
} // writeValue


/**
 * @brief Write a byte value to the Descriptor.
 * @param [in] The single byte to write.
 * @param [in] True if we expect a response.
 */
void BLERemoteDescriptor::writeValue(uint8_t newValue, bool response) {
	writeValue(&newValue, 1, response);
} // writeValue

std::string BLERemoteDescriptor::readValue() {
	// Check to see that we are connected.
	if (!getRemoteCharacteristic()->getRemoteService()->getClient()->isConnected()) {
		return std::string();
	}
	m_semaphoreReadDescrEvt.take("readValue");
	client_attr_read(m_pRemoteCharacteristic->getRemoteService()->getClient()->getConnId(), m_pRemoteCharacteristic->getRemoteService()->getClient()->getGattcIf(),getHandle());
	// Block waiting for the event that indicates that the read has completed.  When it has, the std::string found
	// in m_value will contain our data.
	m_semaphoreReadDescrEvt.wait("readValue");
	return m_value;
} // readValue

uint8_t BLERemoteDescriptor::readUInt8() {
	std::string value = readValue();
	if (value.length() >= 1) {
		return (uint8_t) value[0];
	}
	return 0;
} // readUInt8

uint16_t BLERemoteDescriptor::readUInt16() {
	std::string value = readValue();
	if (value.length() >= 2) {
		return *(uint16_t*) value.data();
	}
	return 0;
} // readUInt16

uint32_t BLERemoteDescriptor::readUInt32() {
	std::string value = readValue();
	if (value.length() >= 4) {
		return *(uint32_t*) value.data();
	}
	return 0;
} // readUInt32



