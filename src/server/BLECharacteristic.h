/*
 * BLECharacteristic.h
 *
 *  Created on: Jun 22, 2017
 *      Author: kolban
 */

#ifndef COMPONENTS_CPP_UTILS_BLECHARACTERISTIC_H_
#define COMPONENTS_CPP_UTILS_BLECHARACTERISTIC_H_
#include <string>
#include <map>
#include "BLEUUID.h"
#include "BLEDescriptor.h"
#include "BLEValue.h"
#include "BLEFreeRTOS.h"
#include "BLEService.h"
typedef uint8_t T_SERVER_ID;

class BLEService;
class BLEDescriptor;
class BLECharacteristicCallbacks;

/**
 * @brief A management structure for %BLE descriptors.
 */
class BLEDescriptorMap {
public:
    void setByUUID(BLEUUID uuid, BLEDescriptor* pDescriptor);
	BLEDescriptor* getByUUID(BLEUUID uuid);
	BLEDescriptor* getFirst();
	BLEDescriptor* getNext();
	std::string	toString();
	void handleGATTServerEvent(T_SERVER_ID service_id, void *p_datas);


private:
	std::map<BLEDescriptor*, std::string> m_uuidMap;
    std::map<BLEDescriptor*, std::string>::iterator m_iterator;
};


/**
 * @brief The model of a %BLE Characteristic.
 *
 * A BLE Characteristic is an identified value container that manages a value. It is exposed by a BLE server and
 * can be read and written to by a %BLE client.
 */
class BLECharacteristic {
public:
    BLECharacteristic(BLEUUID uuid, uint32_t properties = 0);
	void setBroadcastProperty(bool value);
	void setReadProperty(bool value);
	void setWriteProperty(bool value);
	void setNotifyProperty(bool value);
	void setIndicateProperty(bool value);
	void setWriteNoResponseProperty(bool value);

	void setValue(std::string value);
	void setValue(uint8_t* data, size_t size);

	BLEUUID        getUUID();



	static const uint32_t PROPERTY_READ      = 1<<0;
	static const uint32_t PROPERTY_WRITE     = 1<<1;
	static const uint32_t PROPERTY_NOTIFY    = 1<<2;
	static const uint32_t PROPERTY_BROADCAST = 1<<3;
	static const uint32_t PROPERTY_INDICATE  = 1<<4;
	static const uint32_t PROPERTY_WRITE_NR  = 1<<5;

private:
    friend class BLEServer;
	friend class BLEService;
	friend class BLECharacteristicMap;
	friend class BLEDescriptor;

    BLEUUID                     m_bleUUID;
	uint16_t                    m_handle;
    uint8_t                     m_properties;
    BLECharacteristicCallbacks* m_pCallbacks;

	BLEValue                    m_value;
	BLEService*                 m_pService;
	BLEDescriptorMap            m_descriptorMap;

    void                 executeCreate(BLEService* pService);
	void handleGATTServerEvent(T_SERVER_ID service_id, void *p_datas);
	BLEFreeRTOS::Semaphore m_semaphoreCreateEvt = BLEFreeRTOS::Semaphore("CreateEvt");
	BLEFreeRTOS::Semaphore m_semaphoreSetValue  = BLEFreeRTOS::Semaphore("SetValue"); 


}; // BLECharacteristic


/**
 * @brief Callbacks that can be associated with a %BLE characteristic to inform of events.
 *
 * When a server application creates a %BLE characteristic, we may wish to be informed when there is either
 * a read or write request to the characteristic's value. An application can register a
 * sub-classed instance of this class and will be notified when such an event happens.
 */
class BLECharacteristicCallbacks {
public:
	typedef enum {
		SUCCESS_INDICATE,
		SUCCESS_NOTIFY,
		ERROR_INDICATE_DISABLED,
		ERROR_NOTIFY_DISABLED,
		ERROR_GATT,
		ERROR_NO_CLIENT,
		ERROR_INDICATE_TIMEOUT,
		ERROR_INDICATE_FAILURE
	}Status;

};
#endif /* COMPONENTS_CPP_UTILS_BLECHARACTERISTIC_H_ */
