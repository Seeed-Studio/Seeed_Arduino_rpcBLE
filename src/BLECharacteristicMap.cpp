/*
 * BLECharacteristicMap.cpp
 *
 *  Created on: Jun 22, 2017
 *      Author: kolban
 */
#include <sstream>
#include <iomanip>
#include "BLEService.h"
#include "Arduino.h"


/**
 * @brief Set the characteristic by UUID.
 * @param [in] uuid The uuid of the characteristic.
 * @param [in] characteristic The characteristic to cache.
 * @return N/A.
 */
void BLECharacteristicMap::setByUUID(BLECharacteristic* pCharacteristic, BLEUUID uuid) {
	m_uuidMap.insert(std::pair<BLECharacteristic*, std::string>(pCharacteristic, uuid.toString()));
} // setByUUID


/**
 * @brief Set the characteristic by handle.
 * @param [in] handle The handle of the characteristic.
 * @param [in] characteristic The characteristic to cache.
 * @return N/A.
 */
void BLECharacteristicMap::setByHandle(uint16_t handle, BLECharacteristic* characteristic) {
	m_handleMap.insert(std::pair<uint16_t, BLECharacteristic*>(handle, characteristic));
} // setByHandle



/**
 * @brief Return the characteristic by handle.
 * @param [in] handle The handle to look up the characteristic.
 * @return The characteristic.
 */
BLECharacteristic* BLECharacteristicMap::getByHandle(uint16_t handle) {
	return m_handleMap.at(handle);
} // getByHandle

/**
 * @brief Return the characteristic by UUID.
 * @param [in] UUID The UUID to look up the characteristic.
 * @return The characteristic.
 */
BLECharacteristic* BLECharacteristicMap::getByUUID(BLEUUID uuid) {
	for (auto &myPair : m_uuidMap) {
		if (myPair.first->getUUID().equals(uuid)) {
			return myPair.first;
		}
	}
	return nullptr;
} // getByUUID

/**
 * @brief Get the first characteristic in the map.
 * @return The first characteristic in the map.
 */
BLECharacteristic* BLECharacteristicMap::getFirst() {
	m_iterator = m_uuidMap.begin();
	if (m_iterator == m_uuidMap.end()) return nullptr;
	BLECharacteristic* pRet = m_iterator->first;
	m_iterator++;
	return pRet;
} // getFirst


/**
 * @brief Get the next characteristic in the map.
 * @return The next characteristic in the map.
 */
BLECharacteristic* BLECharacteristicMap::getNext() {
	if (m_iterator == m_uuidMap.end()) return nullptr;
	BLECharacteristic* pRet = m_iterator->first;
	m_iterator++;
	return pRet;
} // getNext


/**
 * @brief Pass the GATT server event onwards to each of the characteristics found in the mapping
 * @param [in] event
 * @param [in] gatts_if
 * @param [in] param
 */
void BLECharacteristicMap::handleGATTServerEvent(T_SERVER_ID service_id, void *p_data) {
	// Invoke the handler for every Service we have.
	for (auto& myPair : m_uuidMap) {
		myPair.first->handleGATTServerEvent(service_id,p_data);
	}
} // handleGATTServerEvent



