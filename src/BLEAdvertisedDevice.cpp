/*
 * BLEAdvertisedDevice.cpp
 *
 * During the scanning procedure, we will be finding advertised BLE devices.  This class
 * models a found device.
 *
 *
 * See also:
 * https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile
 *
 *  Created on: Jul 3, 2020
 *      Author: coolc
 */
#define TAG "AdvertisedDevice"
#include <sstream>
#include "BLEAdvertisedDevice.h"
#include "rpc_unified_log.h"


BLEAdvertisedDevice::BLEAdvertisedDevice() {

	m_adFlag           = 0;
	m_appearance       = 0;
	m_deviceType       = 0;
	m_name             = "";
	m_rssi             = -9999;
	m_txPower          = 0;
	m_pScan            = nullptr;
	m_haveAppearance       = false;
	m_haveManufacturerData = false;
	m_haveName             = false;
	m_haveRSSI             = false;
	m_haveServiceData      = false;
	m_haveServiceUUID      = false;
	m_haveTXPower          = false;

} // BLEAdvertisedDevice


T_GAP_REMOTE_ADDR_TYPE BLEAdvertisedDevice::getAddressType() {
	return m_addressType;
}

/**
 * @brief Check advertised serviced for existence required UUID
 * @return Return true if service is advertised
 */
bool BLEAdvertisedDevice::isAdvertisingService(BLEUUID uuid){
	for (int i = 0; i < m_serviceUUIDs.size(); i++) {
		if (m_serviceUUIDs[i].equals(uuid)) return true;
	}
	return false;   
}

/**
 * @brief Get the Service UUID.
 * @return The Service UUID of the advertised device.
 */
BLEUUID BLEAdvertisedDevice::getServiceUUID() {
	return m_serviceUUIDs[0];
} // getServiceUUID

/**
 * @brief Get the Service UUID.
 * @return The Service UUID of the advertised device.
 */
BLEUUID BLEAdvertisedDevice::getServiceUUID(int i) {
	return m_serviceUUIDs[i];
} // getServiceUUID



/**
 * @brief Set the Service UUID for this device.
 * @param [in] serviceUUID The discovered serviceUUID
 */
void BLEAdvertisedDevice::setServiceUUID(BLEUUID serviceUUID) {
	m_serviceUUIDs.push_back(serviceUUID);
	m_haveServiceUUID = true;
} // setServiceUUID


/**
 * @brief Does this advertisement have a service UUID value?
 * @return True if there is a service UUID value present.
 */
bool BLEAdvertisedDevice::haveServiceUUID() {
	return m_haveServiceUUID;
} // haveServiceUUID

/**
 * @brief Get the appearance.
 *
 * A %BLE device can declare its own appearance.  The appearance is how it would like to be shown to an end user
 * typcially in the form of an icon.
 *
 * @return The appearance of the advertised device.
 */
uint16_t BLEAdvertisedDevice::getAppearance() {
	return m_appearance;
} // getAppearance

/**
 * @brief Get the manufacturer data.
 * @return The manufacturer data of the advertised device.
 */
uint8_t* BLEAdvertisedDevice::getManufacturerData() {
	return m_manufacturerData;
} // getManufacturerData


/**
 * @brief Does this advertisement have an appearance value?
 * @return True if there is an appearance value present.
 */
bool BLEAdvertisedDevice::haveAppearance() {
	return m_haveAppearance;
} // haveAppearance

/**
 * @brief Get the address.
 *
 * Every %BLE device exposes an address that is used to identify it and subsequently connect to it.
 * Call this function to obtain the address of the advertised device.
 *
 * @return The address of the advertised device.
 */
BLEAddress BLEAdvertisedDevice::getAddress() {
	return m_address;
} // getAddress

/**
 * @brief Get the name.
 * @return The name of the advertised device.
 */
std::string BLEAdvertisedDevice::getName() {
	return m_name;
} // getName

/**
 * @brief Get the RSSI.
 * @return The RSSI of the advertised device.
 */
int BLEAdvertisedDevice::getRSSI() {
	return m_rssi;
} // getRSSI

/**
 * @brief Get the scan object that created this advertisement.
 * @return The scan object.
 */
BLEScan* BLEAdvertisedDevice::getScan() {
	return m_pScan;
} // getScan


/**
 * @brief Create a string representation of this device.
 * @return A string representation of this device.
 */
std::string BLEAdvertisedDevice::toString() {
	std::string res = "Name: " + getName() + ", Address: " + getAddress().toString();
	if (haveAppearance()) {
		char val[6];
		snprintf(val, sizeof(val), "%d", getAppearance());
		res += ", appearance: ";
		res += val;
	}
	if (haveServiceUUID()) {
		for (int i=0; i < m_serviceUUIDs.size(); i++) {
		    res += ", serviceUUID: " + getServiceUUID(i).toString();
		}
	}
	if (haveTXPower()) {
		char val[4];
		snprintf(val, sizeof(val), "%d", getTXPower());
		res += ", txPower: ";
		res += val;
	}
	return res;
} // toString


/**
 * @brief Does this advertisement have a transmission power value?
 * @return True if there is a transmission power value present.
 */
bool BLEAdvertisedDevice::haveTXPower() {
	return m_haveTXPower;
} // haveTXPower


/**
 * @brief Does this advertisement have manufacturer data?
 * @return True if there is manufacturer data present.
 */
bool BLEAdvertisedDevice::haveManufacturerData() {
	return m_haveManufacturerData;
} // haveManufacturerData

/**
 * @brief Does this advertisement have a signal strength value?
 * @return True if there is a signal strength value present.
 */
bool BLEAdvertisedDevice::haveRSSI() {
	return m_haveRSSI;
} // haveRSSI

/**
 * @brief Get the TX Power.
 * @return The TX Power of the advertised device.
 */
int8_t BLEAdvertisedDevice::getTXPower() {
	return m_txPower;
} // getTXPower


/**
 * @brief Set the address of the advertised device.
 * @param [in] address The address of the advertised device.
 */
void BLEAdvertisedDevice::setAddress(BLEAddress address) {
	m_address = address;
} // setAddress

/**
 * @brief Set the RSSI for this device.
 * @param [in] rssi The discovered RSSI.
 */
void BLEAdvertisedDevice::setRSSI(int rssi) {
	m_rssi     = rssi;
	m_haveRSSI = true;
} // setRSSI


/**
 * @brief Set the Scan that created this advertised device.
 * @param pScan The Scan that created this advertised device.
 */
void BLEAdvertisedDevice::setScan(BLEScan* pScan) {
	m_pScan = pScan;
} // setScan


void BLEAdvertisedDevice::setAddressType(T_GAP_REMOTE_ADDR_TYPE type) {
	m_addressType = type;
}

void BLEAdvertisedDevice::parseAdvertisement(T_LE_CB_DATA *p_data) {
	RPC_DEBUG("Entry parseAdvertisement\n\r");
    T_LE_SCAN_INFO *scan_info = p_data->p_le_scan_info;
    clear();
    _advType = (scan_info->adv_type);
    _addrType = (scan_info->remote_addr_type);
	//set m_address
    BLEAddress advertisedAddress(scan_info->bd_addr);
	m_address = advertisedAddress;
	
    m_rssi = (scan_info->rssi);
    memcpy(m_data, scan_info->data, scan_info->data_len);
    m_dataSize = scan_info->data_len;

    uint8_t buffer[32];
    uint8_t pos = 0;
    while (pos < scan_info->data_len) {
        // Length of the AD structure.
        uint8_t length = scan_info->data[pos++];    // length of following data field = length of advert data field + 1 (adtype)
        uint8_t type;

        if ((length > 0x01) && ((pos + length) <= 31)) {
            // Copy the AD Data to buffer.
            memcpy(buffer, scan_info->data + pos + 1, length - 1);
            // AD Type, one octet.
            type = scan_info->data[pos];
            switch (type) {
                case GAP_ADTYPE_FLAGS: {
                    // (0x01) -- LE Limited Discoverable Mode
                    // (0x02) -- LE General Discoverable Mode
                    // (0x04) -- BR/EDR Not Supported
                    // (0x08) -- Simultaneous LE and BR/EDR to Same Device Capable (Controller)
                    // (0x10) -- Simultaneous LE and BR/EDR to Same Device Capable (Host)
                    m_adFlag = (scan_info->data[(pos + 1)]);
                    break;
                }

                case GAP_ADTYPE_16BIT_MORE:
                case GAP_ADTYPE_16BIT_COMPLETE: {
					RPC_DEBUG("GAP_ADTYPE_16BIT_COMPLETE\n\r");
                    uint8_t *p_uuid = buffer;
                    uint8_t i = length - 1;

                    while (i >= 2) {
                        _serviceList[_serviceCount++] = (BLEUUID(p_uuid, 2));
						setServiceUUID(BLEUUID(p_uuid, 2));
                        p_uuid += 2;
                        i -= 2;
                    }
                    break;
                }

                case GAP_ADTYPE_32BIT_MORE:
                case GAP_ADTYPE_32BIT_COMPLETE: {
                    uint8_t *p_uuid = buffer;
                    uint8_t i = length - 1;
					RPC_DEBUG("GAP_ADTYPE_32BIT_COMPLETE\n\r");
                    while (i >= 4) {
                        _serviceList[_serviceCount++] = (BLEUUID(p_uuid, 4));
						setServiceUUID(BLEUUID(p_uuid, 4));
                        p_uuid += 4;
                        i -= 4;
                    }
                    break;
                }

                case GAP_ADTYPE_128BIT_MORE:
                case GAP_ADTYPE_128BIT_COMPLETE: {
					RPC_DEBUG("GAP_ADTYPE_128BIT_COMPLETE\n\r");
                    uint8_t *p_uuid = buffer;
                    _serviceList[_serviceCount++] = (BLEUUID(p_uuid, 16));
					setServiceUUID(BLEUUID(p_uuid, 16));
                    break;
                }

                case GAP_ADTYPE_LOCAL_NAME_SHORT:
                case GAP_ADTYPE_LOCAL_NAME_COMPLETE: {
                    buffer[length - 1] = '\0';
                    m_name = (std::string((char*)buffer));
                    break;
                }

                case GAP_ADTYPE_POWER_LEVEL: {
                    m_txPower = (scan_info->data[(pos + 1)]);
                    break;
                }

                case GAP_ADTYPE_APPEARANCE: {
                    m_appearance = (((uint16_t)buffer[1] << 8)|(buffer[0]));
                    break;
                }

                case GAP_ADTYPE_MANUFACTURER_SPECIFIC: {
                    uint8_t data_len = length - 3;      // adtype (-1), manufacturerID (-2)
                    m_manufacturer = (((uint16_t)buffer[1] << 8)|(buffer[0]));
                    memcpy(m_manufacturerData, (buffer + 2), data_len);
                    m_manufacturerDataLength = data_len;
                    break;
                }

                default: {
                    uint8_t i = 0;
                    for (i = 0; i < (length - 1); i++) {
                    }
                    break;
                }
            }
        }
        pos += length;
    }			
} // parseAdvertisement

void BLEAdvertisedDevice::clear() {
    memset(m_data, 0, sizeof(m_data));
    m_dataSize = 0;
    m_rssi = 0;
    m_adFlag = 0;
    m_serviceCount = 0;
    m_name = std::string("");
    m_txPower = 0;
    m_appearance = 0;
    m_manufacturer = 0;
    m_manufacturerDataLength = 0;
}



