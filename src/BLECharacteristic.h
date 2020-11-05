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
#include "seeed_rpcUnified.h"
#include "rtl_ble/ble_unified.h"

class BLEService;
class BLEDescriptor;
class BLECharacteristicCallbacks;

/**
 * @brief A management structure for %BLE descriptors.
 */
class BLEDescriptorMap {
public:
    void setByUUID(const char* uuid, BLEDescriptor* pDescriptor);
    void setByUUID(BLEUUID uuid, BLEDescriptor* pDescriptor);
	void setByHandle(uint16_t handle, BLEDescriptor* pDescriptor);
	BLEDescriptor* getByUUID(const char* uuid);
	BLEDescriptor* getByUUID(BLEUUID uuid);
	BLEDescriptor* getFirst();
	BLEDescriptor* getNext();
	BLEDescriptor* getByHandle(uint16_t handle);
	std::string	toString();
	void handleGATTServerEvent(T_SERVER_ID service_id, void *p_data);
 
private:
	std::map<BLEDescriptor*, std::string> m_uuidMap;
	std::map<uint16_t, BLEDescriptor*> m_handleMap;
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
    BLECharacteristic(const char* uuid, uint32_t properties = 0);
    BLECharacteristic(BLEUUID uuid, uint32_t properties = 0);
	virtual ~BLECharacteristic();
	void setBroadcastProperty(bool value);
	void setReadProperty(bool value);
	void setWriteProperty(bool value);
	void setNotifyProperty(bool value);
	void setIndicateProperty(bool value);
	void setWriteNoResponseProperty(bool value);
	void setValue(double& data64);
	void setValue(float& data32);
	void setValue(int& data32);
	void setValue(uint32_t& data32);
	void setValue(uint16_t& data16);
	void setValue(std::string value);
	void setValue(uint8_t* data, size_t size);
	void setAccessPermissions(uint32_t perm);
	void setCallbacks(BLECharacteristicCallbacks* pCallbacks);
	BLEDescriptor* createDescriptor(BLEUUID uuid,uint16_t flags,uint32_t permissions,uint16_t max_len);
	BLEDescriptor* createDescriptor(const char* uuid, uint16_t flags,uint32_t permissions,uint16_t max_len);
	void           addDescriptor(BLEDescriptor* pDescriptor);
	BLEDescriptor* getDescriptorByUUID(const char* descriptorUUID);
	BLEDescriptor* getDescriptorByUUID(BLEUUID descriptorUUID);
	BLEService*    getService();
	std::string    getValue();
	void indicate();
	void notify(bool is_notification = true);
	BLEUUID        getUUID();
	uint8_t*       getData();
	uint8_t        getHandle();
	uint32_t       getAccessPermissions();
	std::string toString();
	static const uint32_t PROPERTY_READ      = GATT_CHAR_PROP_READ;
	static const uint32_t PROPERTY_WRITE     = GATT_CHAR_PROP_WRITE;
	static const uint32_t PROPERTY_NOTIFY    = GATT_CHAR_PROP_NOTIFY;
	static const uint32_t PROPERTY_BROADCAST = GATT_CHAR_PROP_BROADCAST;
	static const uint32_t PROPERTY_INDICATE  = GATT_CHAR_PROP_INDICATE;
	static const uint32_t PROPERTY_WRITE_NR  = GATT_CHAR_PROP_WRITE_NO_RSP;

	static const uint32_t indicationTimeout = 1000;

private:
    friend class BLEServer;
	friend class BLEService;
	friend class BLECharacteristicMap;
	friend class BLEDescriptor;

    BLEUUID                     m_bleUUID;
	uint8_t                     m_handle;
    uint8_t                     m_properties;
	uint32_t                    m_permissions;
    BLECharacteristicCallbacks* m_pCallbacks;


	BLEValue                    m_value;
	BLEService*                 m_pService;
	BLEDescriptorMap            m_descriptorMap;

    void                 executeCreate(BLEService* pService);
	uint8_t              getProperties();
	void handleGATTServerEvent(T_SERVER_ID service_id, void *p_data);
	BLEFreeRTOS::Semaphore m_semaphoreCreateEvt = BLEFreeRTOS::Semaphore("CreateEvt");
	BLEFreeRTOS::Semaphore m_semaphoreSetValue  = BLEFreeRTOS::Semaphore("SetValue");
	BLEFreeRTOS::Semaphore m_semaphoreConfEvt   = BLEFreeRTOS::Semaphore("ConfEvt");



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

	virtual ~BLECharacteristicCallbacks();
	virtual void onRead(BLECharacteristic* pCharacteristic);
	virtual void onWrite(BLECharacteristic* pCharacteristic);
	virtual void onNotify(BLECharacteristic* pCharacteristic);
	virtual void onStatus(BLECharacteristic* pCharacteristic, Status s, uint32_t code);

};
#endif /* COMPONENTS_CPP_UTILS_BLECHARACTERISTIC_H_ */
