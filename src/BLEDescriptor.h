/*
 * BLEDescriptor.h
 *
 *  Created on: Jun 22, 2017
 *      Author: kolban
 */

#ifndef COMPONENTS_CPP_UTILS_BLEDESCRIPTOR_H_
#define COMPONENTS_CPP_UTILS_BLEDESCRIPTOR_H_

#include <string>
#include "BLEUUID.h"
#include "BLECharacteristic.h"
#include "BLEFreeRTOS.h"
#include "BLEValue.h"
#include "seeed_rpcUnified.h"
#include "rtl_ble/ble_unified.h"

typedef uint8_t T_SERVER_ID;
class BLEService;
class BLECharacteristic;
class BLEDescriptorCallbacks;

/**
 * @brief A model of a %BLE descriptor.
 */
class BLEDescriptor {
public:
    BLEDescriptor(BLEUUID uuid,uint16_t flags,uint32_t permissions,uint16_t max_len);
	virtual ~BLEDescriptor();
    BLEUUID  getUUID();
    uint16_t getHandle();
	void setValue(uint8_t* data, size_t size);              // Set the value of the descriptor as a pointer to data.
	void setValue(std::string value);
	void setAccessPermissions(uint32_t perm);	      // Set the permissions of the descriptor.
	void setCallbacks(BLEDescriptorCallbacks* pCallbacks);  // Set callbacks to be invoked for the descriptor.
	uint16_t getflags();
	uint8_t* getValue();
	uint16_t getmaxlen();
	uint32_t getpermissions();
	BLECharacteristic* getCharacteristic();
	void  handleGATTServerEvent(T_SERVER_ID service_id, void *p_data);
	std::string toString();                                 // Convert the descriptor to a string representation.
private:
	friend class BLEDescriptorMap;
	friend class BLECharacteristic;

    BLEValue                m_value;
    BLEUUID                 m_bleUUID;
	uint8_t                 m_handle;
	uint16_t                m_attr_len;
	uint16_t                m_attr_max_len;
	uint8_t*                m_attr_value;
	BLECharacteristic*      m_pCharacteristic;
	BLEDescriptorCallbacks* m_pCallback;

    uint16_t                m_flags;
	uint32_t                m_permissions = 0;

    
	BLEFreeRTOS::Semaphore     m_semaphoreCreateEvt = BLEFreeRTOS::Semaphore("CreateEvt");
	BLEFreeRTOS::Semaphore     m_semaphoreSetValue  = BLEFreeRTOS::Semaphore("SetValue");

	void executeCreate(BLECharacteristic* pCharacteristic);


}; // BLEDescriptor


/**
 * @brief Callbacks that can be associated with a %BLE descriptors to inform of events.
 *
 * When a server application creates a %BLE descriptor, we may wish to be informed when there is either
 * a read or write request to the descriptors value.  An application can register a
 * sub-classed instance of this class and will be notified when such an event happens.
 */
class BLEDescriptorCallbacks {
public:
	virtual ~BLEDescriptorCallbacks();
	virtual void onRead(BLEDescriptor* pDescriptor);
	virtual void onWrite(BLEDescriptor* pDescriptor);
};

#endif /* COMPONENTS_CPP_UTILS_BLEDESCRIPTOR_H_ */
