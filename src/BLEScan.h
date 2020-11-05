/*
 * BLEScan.h
 *
 *  Created on: Jul 1, 2017
 *      Author: kolban
 */

#ifndef COMPONENTS_CPP_UTILS_BLESCAN_H_
#define COMPONENTS_CPP_UTILS_BLESCAN_H_

// #include <vector>
#include <string>
#include "BLEAdvertisedDevice.h"
#include "BLEClient.h"
#include "BLEFreeRTOS.h"
#include "seeed_rpcUnified.h"
#include "rtl_ble/ble_unified.h"

class BLEScan;
class BLEAdvertisedDeviceCallbacks;
class BLEAdvertisedDevice;

/**
 * @brief The result of having performed a scan.
 * When a scan completes, we have a set of found devices.  Each device is described
 * by a BLEAdvertisedDevice object.  The number of items in the set is given by
 * getCount().  We can retrieve a device by calling getDevice() passing in the
 * index (starting at 0) of the desired device.
 */
class BLEScanResults {
public:
	  int                 getCount();
      BLEAdvertisedDevice getDevice(uint32_t i);
private:
	friend BLEScan;
	std::map<std::string, BLEAdvertisedDevice*> m_vectorAdvertisedDevices;
};


class BLEScan {
public:
	void           setActiveScan(bool active);
	void           setInterval(uint16_t intervalMSecs);
	void           setWindow(uint16_t windowMSecs);
	void           setAdvertisedDeviceCallbacks(
			              BLEAdvertisedDeviceCallbacks* pAdvertisedDeviceCallbacks,
										bool wantDuplicates = false);
	bool           start(uint32_t duration, void (*scanCompleteCB)(BLEScanResults), bool is_continue = false);
	BLEScanResults start(uint32_t duration, bool is_continue = false);
	BLEScanResults getResults();
	void		   clearResults();
	void           stop();
	void 		   erase(BLEAddress address);
    
private:
    BLEScan();   // One doesn't create a new instance instead one asks the BLEDevice for the singleton.
	friend class BLEDevice;
	bool                               m_wantDuplicates = false;
	uint8_t                            m_scanMode = GAP_SCAN_MODE_ACTIVE;
	uint16_t                           m_scanInterval = 0x40;              // Duration to wait between starting a scan. Value range: 0x0004 - 0x4000 (2.5ms - 10240ms)(0.625ms/step).
	uint16_t                           m_scanWindow = 0x30;                // Duration to continuously scan for after starting. Value range: 0x0004 - 0x4000 (2.5ms - 10240ms)(0.625ms/step).
	BLEFreeRTOS::Semaphore             m_semaphoreScanEnd = BLEFreeRTOS::Semaphore("ScanEnd");
    void                               updateScanParams();
    T_APP_RESULT                       gapCallbackDefault(uint8_t cb_type, void *p_cb_data);
	BLEScanResults                     m_scanResults;
	void                               (*m_scanCompleteCB)(BLEScanResults scanResults);
	BLEAdvertisedDeviceCallbacks*      m_pAdvertisedDeviceCallbacks = nullptr;
	static uint8_t                     _scanProcessing;	
};

#endif /* COMPONENTS_CPP_UTILS_BLESCAN_H_ */
