/*
 * BLEDescriptor.cpp
 *
 *  Created on: Jun 22, 2017
 *      Author: kolban
 */
#define TAG "BLEDescriptor"
#include <sstream>
#include <string.h>
#include <iomanip>
#include <stdlib.h>
#include "BLEService.h"
#include "BLEDescriptor.h"
#include "rpc_unified_log.h"

#define NULL_HANDLE (0xffff)


/**
 * @brief BLEDescriptor constructor.
 */
BLEDescriptor::BLEDescriptor(BLEUUID uuid,uint16_t flags,uint32_t permissions,uint16_t max_len) {
	m_attr_len           = 0;                                         // Initial length is 0.
	m_handle             = NULL_HANDLE;                               // Handle is initially unknown.
	m_pCharacteristic    = nullptr;                                   // No initial characteristic.
	m_pCallback          = nullptr;                                   // No initial callback
    
	m_bleUUID            = uuid;
	m_flags              = flags;
	m_permissions        = permissions;
	m_attr_max_len       = max_len;                     // Maximum length of the data.
	m_attr_value   = (uint8_t*) malloc(max_len);  // Allocate storage for the value.
} // BLEDescriptor


/**
 * @brief BLEDescriptor destructor.
 */
BLEDescriptor::~BLEDescriptor() {
	free(m_attr_value);   // Release the storage we created in the constructor.
} // ~BLEDescriptor


/**
 * @brief Set the value of the descriptor.
 * @param [in] data The data to set for the descriptor.
 * @param [in] length The length of the data in bytes.
 */
void BLEDescriptor::setValue(uint8_t* data, size_t length) {
	if (length > 600) {
		return;
	}
	m_attr_max_len = length;
	memcpy(m_attr_value, data, length);
} // setValue

/**
 * @brief Set the value of the descriptor.
 * @param [in] value The value of the descriptor in string form.
 */
void BLEDescriptor::setValue(std::string value) {
	setValue((uint8_t*) value.data(), value.length());
} // setValue


/**
 * @brief Get the UUID of the descriptor.
 */
BLEUUID BLEDescriptor::getUUID() {
	return m_bleUUID;
} // getUUID

/**
 * @brief Get the BLE handle for this descriptor.
 * @return The handle for this descriptor.
 */
uint16_t BLEDescriptor::getHandle() {
	return m_handle;
} // getHandle


/**
 * @brief Get the value of this descriptor.
 * @return A pointer to the value of this descriptor.
 */
uint8_t* BLEDescriptor::getValue() {
	return m_attr_value;
} // getValue



uint16_t BLEDescriptor::getflags() {
	return m_flags;
} // m_flags


uint16_t BLEDescriptor::getmaxlen() {
	return m_attr_max_len;
} // m_attr_max_len

uint32_t BLEDescriptor::getpermissions() {
	return m_permissions;
} // m_attr_max_len

/**
 * @brief Execute the creation of the descriptor with the BLE runtime in ESP.
 * @param [in] pCharacteristic The characteristic to which to register this descriptor.
 */
void BLEDescriptor::executeCreate(BLECharacteristic* pCharacteristic) {
	m_pCharacteristic = pCharacteristic; // Save the characteristic associated with this service.
	ble_desc_t desc;
	desc.flags =  getflags();
	desc.uuid_length = getUUID().getNative()->len;
	memcpy(&(desc.uuid), &(getUUID().getNative()->uuid), desc.uuid_length);
	desc.p_value = getValue();
	desc.vlaue_length = getmaxlen();
	desc.permissions =   getpermissions();
	uint8_t desc_handle = ble_create_desc(m_pCharacteristic->getService()->getgiff(), m_pCharacteristic->getHandle(), desc);
 	m_handle = desc_handle;
	RPC_DEBUG("desc_handle: %d\n\r", desc_handle);
} // executeCreate


BLEDescriptorCallbacks::~BLEDescriptorCallbacks() {}

/**
 * @brief Callback function to support a read request.
 * @param [in] pDescriptor The descriptor that is the source of the event.
 */
void BLEDescriptorCallbacks::onRead(BLEDescriptor* pDescriptor) {

} // onRead

/**
 * @brief Callback function to support a write request.
 * @param [in] pDescriptor The descriptor that is the source of the event.
 */
void BLEDescriptorCallbacks::onWrite(BLEDescriptor* pDescriptor) {

} // onWrite



/**
 * @brief Handle GATT server events for the descripttor.
 * @param [in] event
 * @param [in] gatts_if
 * @param [in] param
 */
void BLEDescriptor::handleGATTServerEvent(
		            T_SERVER_ID service_id,
	             void *p_data) {
	ble_service_cb_data_t *cb_data = (ble_service_cb_data_t *)p_data;
    switch (cb_data->event)
    {
    case SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION:
    {
        break;
    }
    case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
    {
        break;
    }
    case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:
    {
        break;
    }
    default:
        break;
    }

} // handleGATTServerEvent