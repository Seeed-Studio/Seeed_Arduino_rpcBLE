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
BLEDescriptor::BLEDescriptor(BLEUUID uuid, uint16_t max_len) {
	m_bleUUID            = uuid;
	m_attr_len     = 0;                                         // Initial length is 0.
	m_attr_max_len = max_len;                     // Maximum length of the data.
	m_handle             = NULL_HANDLE;                               // Handle is initially unknown.
	m_pCharacteristic    = nullptr;                                   // No initial characteristic.
	m_pCallback          = nullptr;                                   // No initial callback.

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


/**
 * @brief Execute the creation of the descriptor with the BLE runtime in ESP.
 * @param [in] pCharacteristic The characteristic to which to register this descriptor.
 */
void BLEDescriptor::executeCreate(BLECharacteristic* pCharacteristic) {

	if (m_handle != NULL_HANDLE) {
		return;
	}

	m_pCharacteristic = pCharacteristic; // Save the characteristic associated with this service.


//	esp_attr_control_t control;
//	control.auto_rsp = ESP_GATT_AUTO_RSP;
	m_semaphoreCreateEvt.take("executeCreate");
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
	m_semaphoreCreateEvt.wait("executeCreate");
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