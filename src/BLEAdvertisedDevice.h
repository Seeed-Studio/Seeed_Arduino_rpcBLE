/*
 * BLEAdvertisedDevice.h
 *
 *  Created on: Jul 3, 2017
 *      Author: kolban
 */

#ifndef COMPONENTS_CPP_UTILS_BLEADVERTISEDDEVICE_H_
#define COMPONENTS_CPP_UTILS_BLEADVERTISEDDEVICE_H_

#include <map>
#include "BLEScan.h"
#include "BLEAddress.h"
#include "BLEUUID.h"
#include <vector>
#include "Seeed_erpcUnified.h"

class BLEScan;





/**
 * @brief A representation of a %BLE advertised device found by a scan.
 *
 * When we perform a %BLE scan, the result will be a set of devices that are advertising.  This
 * class provides a model of a detected device.
 */
class BLEAdvertisedDevice {
public:
    BLEAdvertisedDevice();
    BLEAddress  getAddress();
    std::string getName();
//	BLEUUID     getServiceDataUUID();
//	BLEUUID     getServiceDataUUID(int i);
	BLEUUID     getServiceUUID();
	BLEUUID     getServiceUUID(int i);
	uint16_t    getAppearance();
	int8_t      getTXPower();
	std::string toString();
	
	bool        haveServiceUUID();
	bool        haveTXPower();
	bool        haveName();	
    bool        haveAppearance();
	
	void setAddressType(T_GAP_REMOTE_ADDR_TYPE type);
	
private:
	friend class BLEScan;
    bool m_haveAppearance;
	bool m_haveServiceUUID;
	bool m_haveTXPower;
	bool m_haveManufacturerData;
	bool m_haveName;
	bool m_haveRSSI;
	bool m_haveServiceData;
	
	std::string m_name;
	uint16_t    m_appearance;
	BLEAddress  m_address = BLEAddress((uint8_t*)"\0\0\0\0\0\0");
	std::vector<BLEUUID> m_serviceUUIDs;
	int8_t      m_txPower;
	
    int         m_rssi;
	BLEScan*    m_pScan;
	uint8_t     m_adFlag;
	int         m_deviceType;
	std::string m_manufacturerData;
//	std::vector<std::string> m_serviceData;
	
	void setAddress(BLEAddress address);
	void setRSSI(int rssi);
	void setScan(BLEScan* pScan);
	T_GAP_REMOTE_ADDR_TYPE m_addressType;
};
/**
 * @brief A callback handler for callbacks associated device scanning.
 *
 * When we are performing a scan as a %BLE client, we may wish to know when a new device that is advertising
 * has been found.  This class can be sub-classed and registered such that when a scan is performed and
 * a new advertised device has been found, we will be called back to be notified.
 */
class BLEAdvertisedDeviceCallbacks {
public:
	virtual ~BLEAdvertisedDeviceCallbacks() {}
	/**
	 * @brief Called when a new scan result is detected.
	 *
	 * As we are scanning, we will find new devices.  When found, this call back is invoked with a reference to the
	 * device that was found.  During any individual scan, a device will only be detected one time.
	 */
	virtual void onResult(BLEAdvertisedDevice advertisedDevice) = 0;
};


#endif /* COMPONENTS_CPP_UTILS_BLEADVERTISEDDEVICE_H_ */
