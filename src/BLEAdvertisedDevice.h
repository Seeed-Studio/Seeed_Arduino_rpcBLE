/*
 * BLEAdvertisedDevice.h
 *
 *  Created on: Jul 3, 2020
 *      Author: coolc
 */

#ifndef COMPONENTS_CPP_UTILS_BLEADVERTISEDDEVICE_H_
#define COMPONENTS_CPP_UTILS_BLEADVERTISEDDEVICE_H_

#include <map>
#include "BLEScan.h"
#include "BLEAddress.h"
#include "BLEUUID.h"
#include <vector>
#include "seeed_rpcUnified.h"
#include "rtl_ble/ble_unified.h"

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
	BLEUUID     getServiceUUID();
	BLEUUID     getServiceUUID(int i);
	uint16_t    getAppearance();
	int8_t      getTXPower();
	uint8_t*    getManufacturerData();
	int         getRSSI();
	BLEScan*    getScan();
	std::string toString();
	bool        haveServiceUUID();
	bool        haveTXPower();
	bool        haveName();
    bool        haveAppearance();
	bool        haveRSSI();
	void        setAddressType(T_GAP_REMOTE_ADDR_TYPE type);
	bool		isAdvertisingService(BLEUUID uuid);
	T_GAP_REMOTE_ADDR_TYPE getAddressType();
	bool        haveManufacturerData();
private:
	friend class BLEScan;
	void clear(void);
    bool m_haveAppearance;
	bool m_haveServiceUUID;
	bool m_haveTXPower;
	bool m_haveManufacturerData;
	bool m_haveName;
	bool m_haveRSSI;
	bool m_haveServiceData;
	std::vector<BLEUUID> m_serviceUUIDs;

	T_GAP_ADV_EVT_TYPE _advType;
	T_GAP_REMOTE_ADDR_TYPE _addrType;
	BLEAddress  m_address = BLEAddress((uint8_t*)"\0\0\0\0\0\0");
	
	uint8_t m_data[31] ={0}; // array for storing formatted advertising data for receiving and sending
    uint8_t m_dataSize = 0;
    int         m_rssi;
	BLEScan*    m_pScan;
	uint8_t     m_adFlag;
	uint8_t     m_serviceCount = 0;
	std::string m_name;
	int8_t      m_txPower = 0;
	uint16_t    m_appearance = 0;
    uint16_t    m_manufacturer = 0;
    uint8_t     m_manufacturerData[27] = {0};
    uint8_t     m_manufacturerDataLength = 0;
	BLEUUID     _serviceList[7];        // A 31byte advert can only fit a maximum of 7 service UUIDs of 16bit length
	uint8_t     _serviceCount = 0;	
	int         m_deviceType;	
	void parseAdvertisement(T_LE_CB_DATA *p_data);
	void setAddress(BLEAddress address);
	void setRSSI(int rssi);
	void setScan(BLEScan* pScan);
	void setServiceUUID(BLEUUID serviceUUID);
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
