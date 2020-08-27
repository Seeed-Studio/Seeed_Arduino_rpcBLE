/*
 * BLERemoteService.cpp
 *
 *  Created on: Jul 8, 2017
 *      Author: kolban
 */



#include <sstream>
#include "BLERemoteService.h"
#pragma GCC diagnostic warning "-Wunused-but-set-parameter"
BLERemoteService *BLERemoteService::_this = NULL;

BLERemoteService::BLERemoteService(
	uint16_t    att_handle,
	uint16_t    end_group_handle,
    uint16_t    uuid16,
	BLEClient*    pClient
	) {
	m_srvcId16  = uuid16;
	m_uuid    = BLEUUID(m_srvcId16);
	m_haveCharacteristics = false;
	m_startHandle = att_handle;
	m_endHandle = end_group_handle;
	m_pClient = pClient;
}

BLERemoteService::~BLERemoteService() {
}

/**
 * @brief Get the client associated with this service.
 * @return A reference to the client associated with this service.
 */
BLEClient* BLERemoteService::getClient() {
	return m_pClient;
} // getClient

/**
 * @brief Get the characteristic object for the UUID.
 * @param [in] uuid Characteristic uuid.
 * @return Reference to the characteristic object.
 * @throws BLEUuidNotFoundException
 */
BLERemoteCharacteristic* BLERemoteService::getCharacteristic(BLEUUID uuid) {
	if (!m_haveCharacteristics) {
		retrieveCharacteristics();
	}
	std::string v = uuid.toString();
	for (auto &myPair : m_pClient->m_characteristicMap) {
		if (myPair.first == v) {
			return myPair.second;
		}
	}
	// throw new BLEUuidNotFoundException();  // <-- we dont want exception here, which will cause app crash, we want to search if any characteristic can be found one after another
	return nullptr;
} // getCharacteristic

/**
 * @brief Retrieve all the characteristics for this service.
 * This function will not return until we have all the characteristics.
 * @return N/A
 */
void BLERemoteService::retrieveCharacteristics() {
	removeCharacteristics(); // Forget any previous characteristics.
	client_all_char_discovery(m_pClient->getConnId(), m_pClient->getGattcIf(),m_startHandle,m_endHandle);
    
	BLERemoteService::_this = this;
    m_semaphoregetchaEvt.take("getCharacteristic");	
	m_haveCharacteristics = (m_semaphoregetchaEvt.wait("getCharacteristic") == 0);	

} // getCharacteristics

/**
 * @brief Delete the characteristics in the characteristics map.
 * We maintain a map called m_characteristicsMap that contains pointers to BLERemoteCharacteristic
 * object references.  Since we allocated these in this class, we are also responsible for deleteing
 * them.  This method does just that.
 * @return N/A.
 */
void BLERemoteService::removeCharacteristics() {
	for (auto &myPair : m_pClient->m_characteristicMap) {
	   delete myPair.second;
	   //m_characteristicMap.erase(myPair.first);  // Should be no need to delete as it will be deleted by the clear
	}
	getClient()->m_characteristicMap.clear();   // Clear the map
	for (auto &myPair : m_pClient->m_characteristicMapByHandle) {
	   delete myPair.second;
	}
	m_pClient->m_characteristicMapByHandle.clear();   // Clear the map
} // removeCharacteristics





