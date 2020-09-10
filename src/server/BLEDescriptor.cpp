/*
 * BLEDescriptor.cpp
 *
 *  Created on: Jun 22, 2017
 *      Author: kolban
 */

#include <sstream>
#include <string.h>
#include <iomanip>
#include <stdlib.h>
#include "BLEService.h"
#include "BLEDescriptor.h"

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


uint16_t BLEDescriptor::getflags() {
	return m_flags;
} // m_flags

uint8_t* BLEDescriptor::getattrvalue(){
	return m_attr_value;
}

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

//	if (m_handle != NULL_HANDLE) {
//		return;
//	}

	m_pCharacteristic = pCharacteristic; // Save the characteristic associated with this service.


//	esp_attr_control_t control;
//	control.auto_rsp = ESP_GATT_AUTO_RSP;
//	m_semaphoreCreateEvt.take("executeCreate");
#if 0
	esp_err_t errRc = ::esp_ble_gatts_add_char_descr(
			pCharacteristic->getService()->getHandle(),
			getUUID().getNative(),
			(esp_gatt_perm_t)m_permissions,
			&m_value,
			&control);
	if (errRc != ESP_OK) {
		log_e("<< esp_ble_gatts_add_char_descr: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
		return;
	}
#endif
//	m_semaphoreCreateEvt.wait("executeCreate");
    // Serial.printf("BLEDescriptor   executeCreate start\n\r");
    // ble_desc_t desc;
	// desc.flags = getflags();
	// desc.uuid_length = getUUID().getNative()->len;
	// Serial.printf("%s \n\r", getUUID().toString().c_str());
	Serial.printf("desc.uuid:");
	for(int i = 0; i <  getUUID().getNative()->len; i++)
	{
		Serial.printf("%02x ",getUUID().getNative()->uuid.uuid128[i]);
	}
	Serial.printf("\n\r");

	// memcpy(&(desc.uuid), &(getUUID().getNative()->uuid), desc.uuid_length);
	// Serial.printf("desc.uuid:");
	// for(int i = 0; i < desc.uuid_length; i++)
	// {
	// 	Serial.printf("%02x ", desc.uuid[i]);
	// }
	// Serial.printf("\n\r");
	// Serial.printf("BLEDescriptor   executeCreate start\n\r");
	// desc.p_value = getattrvalue();
    // desc.vlaue_length = getmaxlen();
	// desc.permissions = getpermissions();
    // Serial.printf("BLEDescriptor   executeCreate end\n\r");
	// uint8_t desc_handle1 = ble_create_desc(m_pCharacteristic->getService()->getgiff(), m_pCharacteristic->getHandle(), desc);
	// Serial.printf("BLEDescriptor   executeCreate end\n\r");
	// m_handle = desc_handle1;
	// Serial.printf("desc_handle1: %d\n\r", desc_handle1);

	ble_desc_t desc;
	desc.flags =  getflags();
	desc.uuid_length = getUUID().getNative()->len;
	memcpy(&(desc.uuid), &(getUUID().getNative()->uuid), desc.uuid_length);
	desc.p_value = getattrvalue();
	desc.vlaue_length = getmaxlen();
	desc.permissions =   getpermissions();
	uint8_t desc_handle = ble_create_desc(m_pCharacteristic->getService()->getgiff(), m_pCharacteristic->getHandle(), desc);
 	m_handle = desc_handle;
	Serial.printf("desc_handle: %d\n\r", desc_handle);

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
	    void *p_datas) {

} // handleGATTServerEvent