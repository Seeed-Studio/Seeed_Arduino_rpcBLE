/*
 * BLEAdvertising.cpp
 *
 * This class encapsulates advertising a BLE Server.
 *  Created on: Jun 21, 2017
 *      Author: kolban
 *
 * The ESP-IDF provides a framework for BLE advertising.  It has determined that there are a common set
 * of properties that are advertised and has built a data structure that can be populated by the programmer.
 * This means that the programmer doesn't have to "mess with" the low level construction of a low level
 * BLE advertising frame.  Many of the fields are determined for us while others we can set before starting
 * to advertise.
 *
 * Should we wish to construct our own payload, we can use the BLEAdvertisementData class and call the setters
 * upon it.  Once it is populated, we can then associate it with the advertising and what ever the programmer
 * set in the data will be advertised.
 *
 */

#include "BLEAdvertising.h"
/**
 * @brief Construct a default advertising object.
 *
 */
BLEAdvertising::BLEAdvertising() {
	m_advData.set_scan_rsp        = false;
	m_advData.include_name        = true;
	m_advData.include_txpower     = true;
	m_advData.min_interval        = 0x20;
	m_advData.max_interval        = 0x40;
	m_advData.appearance          = 0x00;
	m_advData.manufacturer_len    = 0;
	m_advData.p_manufacturer_data = nullptr;
	m_advData.service_data_len    = 0;
	m_advData.p_service_data      = nullptr;
	m_advData.service_uuid_len    = 0;
	m_advData.p_service_uuid      = nullptr;
	m_advData.flag                = ((0x01 << 1) | (0x01 << 2));


	m_adv_int_min       = 0x20;
	m_adv_int_max       = 0x40;
#if 0
	m_advParams.adv_type          = ADV_TYPE_IND;
	m_advParams.own_addr_type     = BLE_ADDR_TYPE_PUBLIC;
	m_advParams.channel_map       = ADV_CHNL_ALL;
	m_advParams.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;
	m_advParams.peer_addr_type    = BLE_ADDR_TYPE_PUBLIC;
#endif
	m_customAdvData               = false;   // No custom advertising data
	m_customScanResponseData      = false;   // No custom scan response data
} // BLEAdvertising



/**
 * @brief Add a service uuid to exposed list of services.
 * @param [in] serviceUUID The UUID of the service to expose.
 */
void BLEAdvertising::addServiceUUID(BLEUUID serviceUUID) {
	m_serviceUUIDs.push_back(serviceUUID);
} // addServiceUUID


void BLEAdvertising::setScanResponse(bool set) {
	m_scanResp = set;
}

void BLEAdvertising::setMinPreferred(uint16_t mininterval) {
	m_advData.min_interval = mininterval;
} //



/**
 * @brief Start advertising.
 * Start advertising.
 * @return N/A.
 */
void BLEAdvertising::start() {
	int numServices = m_serviceUUIDs.size();
	if (numServices > 0) {
		m_advData.service_uuid_len = 16 * numServices;
		m_advData.p_service_uuid = new uint8_t[m_advData.service_uuid_len];
		uint8_t* p = m_advData.p_service_uuid;
		for (int i = 0; i < numServices; i++) {
			BLEUUID serviceUUID128 = m_serviceUUIDs[i].to128();
			memcpy(p, serviceUUID128.getNative()->uuid.uuid128, 16);
			p += 16;
		}
	} else {
		m_advData.service_uuid_len = 0;
	}

	if (!m_customAdvData) {
		// Set the configuration for advertising.
		m_advData.set_scan_rsp = false;
		m_advData.include_name = !m_scanResp;
		m_advData.include_txpower = !m_scanResp;

#if 0
//*********************配置广播数据********************************************************************
		errRc = ::esp_ble_gap_config_adv_data(&m_advData);
		if (errRc != ESP_OK) {
			log_e("<< esp_ble_gap_config_adv_data: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
			return;
		}
#endif 
	}

	if (!m_customScanResponseData && m_scanResp) {
		m_advData.set_scan_rsp = true;
		m_advData.include_name = m_scanResp;
		m_advData.include_txpower = m_scanResp;
#if 0
		errRc = ::esp_ble_gap_config_adv_data(&m_advData);
		if (errRc != ESP_OK) {
			log_e("<< esp_ble_gap_config_adv_data (Scan response): rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
			return;
		}
#endif 
	}

	// If we had services to advertise then we previously allocated some storage for them.
	// Here we release that storage.
	if (m_advData.service_uuid_len > 0) {
		delete[] m_advData.p_service_uuid;
		m_advData.p_service_uuid = nullptr;
	}

#if 0
	// *****************************Start advertising.*********************************************
	errRc = ::esp_ble_gap_start_advertising(&m_advParams);
	if (errRc != ESP_OK) {
		log_e("<< esp_ble_gap_start_advertising: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
		return;
	}
	log_v("<< start");
#endif 
} // start