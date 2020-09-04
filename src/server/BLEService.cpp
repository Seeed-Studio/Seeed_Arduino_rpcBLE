/*
 * BLEService.cpp
 *
 *  Created on: Mar 25, 2017
 *      Author: kolban
 */

// A service is identified by a UUID.  A service is also the container for one or more characteristics

#include <iomanip>
#include <sstream>
#include <string>

#include "BLEServer.h"
#include "BLEService.h"

#define NULL_HANDLE (0xffff)

/**
 * @brief Construct an instance of the BLEService
 * @param [in] uuid The UUID of the service.
 * @param [in] numHandles The maximum number of handles associated with the service.
 */
BLEService::BLEService(BLEUUID uuid, uint16_t numHandles) {
	m_uuid      = uuid;
	m_handle    = NULL_HANDLE;
	m_pServer   = nullptr;
	//m_serializeMutex.setName("BLEService");
	m_lastCreatedCharacteristic = nullptr;
	m_numHandles = numHandles;
} // BLEService


/**
 * @brief Create the service.
 * Create the service.
 * @param [in] gatts_if The handle of the GATT server interface.
 * @return N/A.
 */

void BLEService::executeCreate(BLEServer* pServer) {
	m_pServer          = pServer;
	m_semaphoreCreateEvt.take("executeCreate"); // Take the mutex and release at event ESP_GATTS_CREATE_EVT

#if 0
	esp_gatt_srvc_id_t srvc_id;
	srvc_id.is_primary = true;
	srvc_id.id.inst_id = m_instId;
	srvc_id.id.uuid    = *m_uuid.getNative();
	esp_err_t errRc = ::esp_ble_gatts_create_service(getServer()->getGattsIf(), &srvc_id, m_numHandles); // The maximum number of handles associated with the service.
#endif

	m_semaphoreCreateEvt.wait("executeCreate");
} // executeCreate


/**
 * @brief Create a new BLE Characteristic associated with this service.
 * @param [in] uuid - The UUID of the characteristic.
 * @param [in] properties - The properties of the characteristic.
 * @return The new BLE characteristic.
 */
BLECharacteristic* BLEService::createCharacteristic(BLEUUID uuid, uint32_t properties) {
	BLECharacteristic* pCharacteristic = new BLECharacteristic(uuid, properties);
	addCharacteristic(pCharacteristic);
	return pCharacteristic;
} // createCharacteristic


/**
 * @brief Add a characteristic to the service.
 * @param [in] pCharacteristic A pointer to the characteristic to be added.
 */
void BLEService::addCharacteristic(BLECharacteristic* pCharacteristic) {
	// We maintain a mapping of characteristics owned by this service.  These are managed by the
	// BLECharacteristicMap class instance found in m_characteristicMap.  We add the characteristic
	// to the map and then ask the service to add the characteristic at the BLE level (ESP-IDF).

	// Check that we don't add the same characteristic twice.
	if (m_characteristicMap.getByUUID(pCharacteristic->getUUID()) != nullptr) {	

		return;
	}

	// Remember this characteristic in our map of characteristics.  At this point, we can lookup by UUID
	// but not by handle.  The handle is allocated to us on the ESP_GATTS_ADD_CHAR_EVT.
	m_characteristicMap.setByUUID(pCharacteristic, pCharacteristic->getUUID());

} // addCharacteristic



/**
 * @brief Get the UUID of the service.
 * @return the UUID of the service.
 */
BLEUUID BLEService::getUUID() {
	return m_uuid;
} // getUUID


/**
 * @brief Get the handle associated with this service.
 * @return The handle associated with this service.
 */
uint16_t BLEService::getHandle() {
	return m_handle;
} // getHandle


/**
 * @brief Return a string representation of this service.
 * A service is defined by:
 * * Its UUID
 * * Its handle
 * @return A string representation of this service.
 */
std::string BLEService::toString() {
	std::string res = "UUID: " + getUUID().toString();
	char hex[5];
	snprintf(hex, sizeof(hex), "%04x", getHandle());
	res += ", handle: 0x";
	res += hex;
	return res;
} // toString


/**
 * @brief Start the service.
 * Here we wish to start the service which means that we will respond to partner requests about it.
 * Starting a service also means that we can create the corresponding characteristics.
 * @return Start the service.
 */
void BLEService::start() {
// We ask the BLE runtime to start the service and then create each of the characteristics.
// We start the service through its local handle which was returned in the ESP_GATTS_CREATE_EVT event
// obtained as a result of calling esp_ble_gatts_create_service().

	if (m_handle == NULL_HANDLE) {
		return;
	}

	BLECharacteristic *pCharacteristic = m_characteristicMap.getFirst();

	while (pCharacteristic != nullptr) {
		m_lastCreatedCharacteristic = pCharacteristic;
		pCharacteristic->executeCreate(this);

		pCharacteristic = m_characteristicMap.getNext();
	}
	// Start each of the characteristics ... these are found in the m_characteristicMap.

	m_semaphoreStartEvt.take("start");

	//****************************启动service服务***************************************************************
	//esp_err_t errRc = ::esp_ble_gatts_start_service(m_handle);

	
	m_semaphoreStartEvt.wait("start");
} // start


/**
 * @brief Handle a GATTS server event.
 */
void BLEService::handleGATTServerEvent(T_SERVER_ID service_id, void *p_datas) {

	
	// Invoke the GATTS handler in each of the associated characteristics.
	m_characteristicMap.handleGATTServerEvent(service_id,p_datas);
} // handleGATTServerEvent