/*
 * BLEService.h
 *
 *  Created on: Mar 25, 2017
 *      Author: kolban
 */

#ifndef COMPONENTS_CPP_UTILS_BLESERVICE_H_
#define COMPONENTS_CPP_UTILS_BLESERVICE_H_

#include "BLECharacteristic.h"
#include "BLEServer.h"
#include "BLEUUID.h"
#include "BLEFreeRTOS.h"

class BLEServer;

/**
 * @brief A data mapping used to manage the set of %BLE characteristics known to the server.
 */
class BLECharacteristicMap {
public:
    void setByUUID(BLECharacteristic* pCharacteristic, BLEUUID uuid);
	BLECharacteristic* getByUUID(BLEUUID uuid);
	BLECharacteristic* getFirst();
	BLECharacteristic* getNext();


private:
	std::map<BLECharacteristic*, std::string> m_uuidMap;
	std::map<BLECharacteristic*, std::string>::iterator m_iterator;

};


/**
 * @brief The model of a %BLE service.
 *
 */
class BLEService {
public:
    void               addCharacteristic(BLECharacteristic* pCharacteristic);
	BLEUUID            getUUID();
	uint16_t           getHandle();
	void               executeCreate(BLEServer* pServer);
	BLECharacteristic* createCharacteristic(BLEUUID uuid, uint32_t properties);
	void               start();
	std::string toString();
	uint8_t			   m_instId = 0;

private:
    BLEService(BLEUUID uuid, uint16_t numHandles);
    friend class BLEServer;
	friend class BLEServiceMap;
	friend class BLEDevice;

    BLEUUID              m_uuid;
	uint16_t             m_handle;
	BLECharacteristic*   m_lastCreatedCharacteristic = nullptr;
	BLEServer*           m_pServer = nullptr;
	uint16_t             m_numHandles;

    BLECharacteristicMap m_characteristicMap;
	BLEFreeRTOS::Semaphore  m_semaphoreCreateEvt = BLEFreeRTOS::Semaphore("CreateEvt");
	BLEFreeRTOS::Semaphore  m_semaphoreStartEvt  = BLEFreeRTOS::Semaphore("StartEvt");

}; // BLEService

#endif /* COMPONENTS_CPP_UTILS_BLESERVICE_H_ */
