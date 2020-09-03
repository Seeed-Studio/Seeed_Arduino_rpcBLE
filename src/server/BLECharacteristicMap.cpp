/*
 * BLECharacteristicMap.cpp
 *
 *  Created on: Jun 22, 2017
 *      Author: kolban
 */
#include <sstream>
#include <iomanip>
#include "BLEService.h"


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
