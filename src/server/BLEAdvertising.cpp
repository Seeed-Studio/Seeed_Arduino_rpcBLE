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
#if 0
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

	m_advParams.adv_type          = ADV_TYPE_IND;
	m_advParams.own_addr_type     = BLE_ADDR_TYPE_PUBLIC;
	m_advParams.channel_map       = ADV_CHNL_ALL;
	m_advParams.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;
	m_advParams.peer_addr_type    = BLE_ADDR_TYPE_PUBLIC;

	m_customAdvData               = false;   // No custom advertising data
	m_customScanResponseData      = false;   // No custom scan response data
#endif
} // BLEAdvertising


void BLEAdvertising::addData(const uint8_t* data, uint8_t size, ble_adv_data_type type) {
	if (type == adv_data)
	{
		if ((31 - _dataSize) < size) {
        printf("Insufficient space in advertising data packet\r\n");
        return;
        }
        int i;
        for (i = 0; i < size; i++) {
        _data[_dataSize] = data[i];
        _dataSize++;
        }
	}else if (type == adv_scan_data)
	{
		if ((31 - scan_dataSize) < size) {
        printf("Insufficient space in advertising data packet\r\n");
        return;
        }
        int i;
        for (i = 0; i < size; i++) {
        scan_data[scan_dataSize] = data[i];
        scan_dataSize++;
        }
	}
}

/**
 * @brief Add a service uuid to exposed list of services.
 * @param [in] serviceUUID The UUID of the service to expose.
 */
uint8_t BLEAdvertising::addServiceUUID(BLEUUID serviceUUID) {
//	m_serviceUUIDs.push_back(serviceUUID);
    _serviceList[_serviceCount++] = (serviceUUID);
    switch (serviceUUID.length()) {
        case 2: {
            uint8_t data[4] = {3, GAP_ADTYPE_16BIT_COMPLETE};
            memcpy(&(data[2]), serviceUUID.dataNative(), 2);
            addData(data, 4, adv_scan_data);
            break;
        }
        case 4: {
            uint8_t data[6] = {5, GAP_ADTYPE_32BIT_COMPLETE};
            memcpy(&(data[2]), serviceUUID.dataNative(), 4);
            addData(data, 6, adv_scan_data);
            break;
        }
        case 16: {
            uint8_t data[18] = {17, GAP_ADTYPE_128BIT_COMPLETE};
            memcpy(&(data[2]), serviceUUID.dataNative(), 16);
            addData(data, 18, adv_scan_data);
            break;
        }
        default:
            break;
    }
    return scan_dataSize;   
} // addServiceUUID


void BLEAdvertising::setScanResponse(bool set) {
//	m_scanResp = set;
    if (set == true)
	{
		memcpy(_scanRspData, scan_data, scan_dataSize);
        _scanRspDataSize = scan_dataSize;
	}
}

void BLEAdvertising::setMinPreferred(uint16_t mininterval) {
//	m_advData.min_interval = mininterval;
    if ((mininterval >= 20) && (mininterval <= 10240)) {
        _advIntMin = (mininterval*1000/625);
    }
} //





/**
 * @brief Start advertising.
 * Start advertising.
 * @return N/A.
 */
void BLEAdvertising::start() {

	addFlags(GAP_ADTYPE_FLAGS_LIMITED | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED);
	addCompleteName("SEEED_BLE_DEV");
	setAdvData();
#if 1
	le_adv_set_param(GAP_PARAM_ADV_EVENT_TYPE, sizeof(_advEvtType), &(_advEvtType));
    le_adv_set_param(GAP_PARAM_ADV_DIRECT_ADDR_TYPE, sizeof(_advDirectType), &(_advDirectType));
    le_adv_set_param(GAP_PARAM_ADV_DIRECT_ADDR, sizeof(_advDirectAddr), (_advDirectAddr));
    le_adv_set_param(GAP_PARAM_ADV_CHANNEL_MAP, sizeof(_advChannMap), &(_advChannMap));
    le_adv_set_param(GAP_PARAM_ADV_FILTER_POLICY, sizeof(_advFilterPolicy), &(_advFilterPolicy));
    le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MIN, sizeof(_advIntMin), &(_advIntMin));
    le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MAX, sizeof(_advIntMax), &(_advIntMax));
    le_adv_set_param(GAP_PARAM_ADV_DATA, _advDataSize, _advData);
    le_adv_set_param(GAP_PARAM_SCAN_RSP_DATA, _scanRspDataSize, _scanRspData);

    le_set_gap_param(GAP_PARAM_SLAVE_INIT_GATT_MTU_REQ, sizeof(_slaveInitMtuReq), &_slaveInitMtuReq);
#endif
    le_adv_start();
    
} // start

uint8_t BLEAdvertising::addFlags(uint8_t flags) {
    uint8_t data[3] = {2, GAP_ADTYPE_FLAGS, flags};
    addData(data, 3, adv_data);
    return _dataSize;
}

uint8_t BLEAdvertising::addCompleteName(const char* str) {
    _devName = String(str);
    uint8_t length = _devName.length();
    uint8_t data[(2 + length)] = {(uint8_t)(1 + length), GAP_ADTYPE_LOCAL_NAME_COMPLETE};
    memcpy(&(data[2]), str, length);
    addData(data, (2 + length), adv_data);
    return _dataSize;
}


uint8_t BLEAdvertising::addShortName(const char* str) {
    _devName = String(str);
    uint8_t length = _devName.length();
    uint8_t data[(2 + length)] = {(uint8_t)(1 + length), GAP_ADTYPE_LOCAL_NAME_SHORT};
    memcpy(&(data[2]), str, length);
    addData(data, (2 + length), adv_data);
    return _dataSize;
}

void BLEAdvertising::setAdvData() {
    memcpy(_advData, _data, _dataSize);
    _advDataSize = _dataSize;
}