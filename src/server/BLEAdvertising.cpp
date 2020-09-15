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
bool ble_start_flags  = false;

/**
 * @brief Construct a default advertising object.
 *
 */
BLEAdvertising::BLEAdvertising() {

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
 * @param [in] serviceUUID The string representation of the service to expose.
 */
void BLEAdvertising::addServiceUUID(const char* serviceUUID) {
	BLEUUID bbb = BLEUUID(serviceUUID);
	Serial.printf("service uuid: \n\r");
	Serial.println(bbb.toString().c_str());
	addServiceUUID(BLEUUID(serviceUUID));
} // addServiceUUID

/**
 * @brief Add a service uuid to exposed list of services.
 * @param [in] serviceUUID The UUID of the service to expose.
 */
uint8_t BLEAdvertising::addServiceUUID(BLEUUID serviceUUID) {
//	m_serviceUUIDs.push_back(serviceUUID);
    _serviceList[_serviceCount++] = (serviceUUID);
	Serial.printf("_serviceList[_serviceCount++] \n\r");
	Serial.printf("serviceUUID.getNative()->len: %d \n\r",serviceUUID.getNative()->len);
    switch (serviceUUID.getNative()->len) {
        case 2: {
            uint8_t data[4] = {3, GAP_ADTYPE_16BIT_COMPLETE};
            memcpy(&(data[2]), &(serviceUUID.getNative()->uuid), 2);
            addData(data, 4, adv_scan_data);
            break;
        }
        case 4: {
            uint8_t data[6] = {5, GAP_ADTYPE_32BIT_COMPLETE};
            memcpy(&(data[2]), &(serviceUUID.getNative()->uuid), 4);
            addData(data, 6, adv_scan_data);
            break;
        }
        case 16: {
            uint8_t data[18] = {17, GAP_ADTYPE_128BIT_COMPLETE};
            memcpy(&(data[2]), &(serviceUUID.getNative()->uuid), 16);  
            addData(data, 18, adv_scan_data);
			Serial.printf("data:");
	        for(int i = 0; i < 18; i++)
	        {
		       Serial.printf("%02x ",data[i]);
	        }
	        Serial.printf("\n\r");

            break;
        }
        default:
            break;
    }
	Serial.printf("scan_dataSize:%d\n\r",scan_dataSize);
    return scan_dataSize;   
} // addServiceUUID


void BLEAdvertising::setScanResponse(bool set) {
//	m_scanResp = set;
    if (set == true)
	{
		
		memcpy(_scanRspData, scan_data, scan_dataSize);
		Serial.printf("_scanRspData:");
	    for(int i = 0; i < 31; i++)
	    {
		    Serial.printf("%02x ",_scanRspData[i]);
	    }
	    Serial.printf("\n\r");
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
 * @brief Set the advertisement data that is to be published in a regular advertisement.
 * @param [in] advertisementData The data to be advertised.
 */
void BLEAdvertising::setAdvertisementData(BLEAdvertisementData& advertisementData) {
#if 0
	log_v(">> setAdvertisementData");
	esp_err_t errRc = ::esp_ble_gap_config_adv_data_raw(
		(uint8_t*)advertisementData.getPayload().data(),
		advertisementData.getPayload().length());
	if (errRc != ESP_OK) {
		log_e("esp_ble_gap_config_adv_data_raw: %d %s", errRc, GeneralUtils::errorToString(errRc));
	}
	m_customAdvData = true;   // Set the flag that indicates we are using custom advertising data.
	log_v("<< setAdvertisementData");
#endif
    memcpy(_advData, (uint8_t*)advertisementData.getPayload().data(), advertisementData.getPayload().length());
} // setAdvertisementData


/**
 * @brief Set the advertisement data that is to be published in a scan response.
 * @param [in] advertisementData The data to be advertised.
 */
void BLEAdvertising::setScanResponseData(BLEAdvertisementData& advertisementData) {
#if 0
	log_v(">> setScanResponseData");
	esp_err_t errRc = ::esp_ble_gap_config_scan_rsp_data_raw(
		(uint8_t*)advertisementData.getPayload().data(),
		advertisementData.getPayload().length());
	if (errRc != ESP_OK) {
		log_e("esp_ble_gap_config_scan_rsp_data_raw: %d %s", errRc, GeneralUtils::errorToString(errRc));
	}
	m_customScanResponseData = true;   // Set the flag that indicates we are using custom scan response data.
	log_v("<< setScanResponseData");
#endif 
    memcpy(_scanRspData, (uint8_t*)advertisementData.getPayload().data(), advertisementData.getPayload().length());
    //le_adv_set_param(GAP_PARAM_SCAN_RSP_DATA, advertisementData.getPayload().length(), (uint8_t*)advertisementData.getPayload().data());
   // setScanRspData(beacon.getScanRsp(), beacon.scanRspSize);
} // setScanResponseData


void BLEAdvertising::setAdvertisementType(uint8_t advType){
	if (advType <= GAP_ADTYPE_ADV_LDC_DIRECT_IND) {
        _advEvtType = advType;
    }
} // setAdvertisementType

/**
 * @brief Start advertising.
 * Start advertising.
 * @return N/A.
 */
void BLEAdvertising::start() {
	
	addFlags(GAP_ADTYPE_FLAGS_LIMITED | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED);
	addCompleteName("SEEED_BLE_DEV");
	setAdvData();
	Serial.printf("BLEAdvertising::start()\n\r");

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
	Serial.printf("BLEAdvertising::end()\n\r");
    if (!ble_start_flags)
	{
		ble_start_flags = true;
		ble_start();
	}
    le_adv_start();
	Serial.printf("BLEAdvertising::end()\n\r");
    
} // start

/**
 * @brief Stop advertising.
 * Stop advertising.
 * @return N/A.
 */
void BLEAdvertising::stop() {
	le_adv_stop();
} // stop

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


/**
 * @brief Set the appearance.
 * @param [in] appearance The appearance code value.
 *
 * See also:
 * https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.gap.appearance.xml
 */
void BLEAdvertisementData::setAppearance(uint16_t appearance) {
	char cdata[2];
	cdata[0] = 3;
	cdata[1] = 0x19; // 0x19
	addData(std::string(cdata, 2) + std::string((char*) &appearance, 2));
} // setAppearance

/**
 * @brief Set the complete services.
 * @param [in] uuid The single service to advertise.
 */
void BLEAdvertisementData::setCompleteServices(BLEUUID uuid) {
	char cdata[2];
	switch (uuid.bitSize()) {
		case 16: {
			// [Len] [0x02] [LL] [HH]
			cdata[0] = 3;
			cdata[1] = 0x03;  // 0x03
			addData(std::string(cdata, 2) + std::string((char*) &uuid.getNative()->uuid.uuid16, 2));
			break;
		}

		case 32: {
			// [Len] [0x04] [LL] [LL] [HH] [HH]
			cdata[0] = 5;
			cdata[1] = 0x05;  // 0x05
			addData(std::string(cdata, 2) + std::string((char*) &uuid.getNative()->uuid.uuid32, 4));
			break;
		}

		case 128: {
			// [Len] [0x04] [0] [1] ... [15]
			cdata[0] = 17;
			cdata[1] = 0x07;  // 0x07
			addData(std::string(cdata, 2) + std::string((char*) uuid.getNative()->uuid.uuid128, 16));
			break;
		}

		default:
			return;
	}
} // setCompleteServices


void BLEAdvertisementData::setFlags(uint8_t flag) {
	char cdata[3];
	cdata[0] = 2;
	cdata[1] = 0x01;  // 0x01
	cdata[2] = flag;
	addData(std::string(cdata, 3));
} // setFlag


/**
 * @brief Set manufacturer specific data.
 * @param [in] data Manufacturer data.
 */
void BLEAdvertisementData::setManufacturerData(std::string data) {
	char cdata[2];
	cdata[0] = data.length() + 1;
	cdata[1] = 0xff;  // 0xff
	addData(std::string(cdata, 2) + data);
} // setManufacturerData

/**
 * @brief Set the name.
 * @param [in] The complete name of the device.
 */
void BLEAdvertisementData::setName(std::string name) {
	char cdata[2];
	cdata[0] = name.length() + 1;
	cdata[1] = 0x09;  // 0x09
	addData(std::string(cdata, 2) + name);
} // setName


/**
 * @brief Set the partial services.
 * @param [in] uuid The single service to advertise.
 */
void BLEAdvertisementData::setPartialServices(BLEUUID uuid) {
	char cdata[2];
	switch (uuid.bitSize()) {
		case 16: {
			// [Len] [0x02] [LL] [HH]
			cdata[0] = 3;
			cdata[1] = 0x02;  // 0x02
			addData(std::string(cdata, 2) + std::string((char *) &uuid.getNative()->uuid.uuid16, 2));
			break;
		}

		case 32: {
			// [Len] [0x04] [LL] [LL] [HH] [HH]
			cdata[0] = 5;
			cdata[1] = 0x04; // 0x04
			addData(std::string(cdata, 2) + std::string((char *) &uuid.getNative()->uuid.uuid32, 4));
			break;
		}

		case 128: {
			// [Len] [0x04] [0] [1] ... [15]
			cdata[0] = 17;
			cdata[1] = 0x06;  // 0x06
			addData(std::string(cdata, 2) + std::string((char *) &uuid.getNative()->uuid.uuid128, 16));
			break;
		}

		default:
			return;
	}
} // setPartialServices

/**
 * @brief Set the service data (UUID + data)
 * @param [in] uuid The UUID to set with the service data.  Size of UUID will be used.
 * @param [in] data The data to be associated with the service data advert.
 */
void BLEAdvertisementData::setServiceData(BLEUUID uuid, std::string data) {
	char cdata[2];
	switch (uuid.bitSize()) {
		case 16: {
			// [Len] [0x16] [UUID16] data
			cdata[0] = data.length() + 3;
			cdata[1] = 0x16;  // 0x16
			addData(std::string(cdata, 2) + std::string((char*) &uuid.getNative()->uuid.uuid16, 2) + data);
			break;
		}

		case 32: {
			// [Len] [0x20] [UUID32] data
			cdata[0] = data.length() + 5;
			cdata[1] = 0x20; // 0x20
			addData(std::string(cdata, 2) + std::string((char*) &uuid.getNative()->uuid.uuid32, 4) + data);
			break;
		}

		case 128: {
			// [Len] [0x21] [UUID128] data
			cdata[0] = data.length() + 17;
			cdata[1] = 0x21;  // 0x21
			addData(std::string(cdata, 2) + std::string((char*) &uuid.getNative()->uuid.uuid128, 16) + data);
			break;
		}

		default:
			return;
	}
} // setServiceData


/**
 * @brief Set the short name.
 * @param [in] The short name of the device.
 */
void BLEAdvertisementData::setShortName(std::string name) {
	char cdata[2];
	cdata[0] = name.length() + 1;
	cdata[1] = 0x08;  // 0x08
	addData(std::string(cdata, 2) + name);
} // setShortName

/**
 * @brief Add data to the payload to be advertised.
 * @param [in] data The data to be added to the payload.
 */
void BLEAdvertisementData::addData(std::string data) {
	if ((m_payload.length() + data.length()) > 31) {
		return;
	}
	m_payload.append(data);
} // addData

/**
 * @brief Retrieve the payload that is to be advertised.
 * @return The payload that is to be advertised.
 */
std::string BLEAdvertisementData::getPayload() {
	return m_payload;
} // getPayload
