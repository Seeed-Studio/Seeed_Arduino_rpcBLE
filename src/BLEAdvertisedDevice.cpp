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
 *  Created on: Jul 3, 2017
 *      Author: kolban
 */


#include <sstream>
#include "BLEAdvertisedDevice.h"

BLEAdvertisedDevice::BLEAdvertisedDevice() {
	m_adFlag           = 0;
	m_appearance       = 0;
	m_deviceType       = 0;
	m_manufacturerData = "";
	m_name             = "";
	m_rssi             = -9999;
//	m_serviceData      = {};
//	m_serviceDataUUIDs = {};
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

#if 0
/**
 * @brief Get the service data UUID.
 * @return The service data UUID.
 */
BLEUUID BLEAdvertisedDevice::getServiceDataUUID() {
	return m_serviceDataUUIDs[0];
} // getServiceDataUUID

/**
 * @brief Get the service data UUID.
 * @return The service data UUID.
 */
BLEUUID BLEAdvertisedDevice::getServiceDataUUID(int i) {
	return m_serviceDataUUIDs[i];
} // getServiceDataUUID

#endif

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




