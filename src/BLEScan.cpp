/*
 * BLEScan.cpp
 *
 *  Created on: Jul 1, 2020
 *      Author: coolc
 */

#define TAG "Scan"
#include <map>
#include <Arduino.h>
#include "BLEScan.h"
#include <functional>
#include "BLEDevice.h"
#include "rpc_unified_log.h"
/**
 * Constructor
 */

BLEScan::BLEScan() {
} // BLEScan



/**
 * @brief Should we perform an active or passive scan?
 * The default is a passive scan.  An active scan means that we will wish a scan response.
 * @param [in] active If true, we perform an active scan otherwise a passive scan.
 * @return N/A.
 */
void BLEScan::setActiveScan(bool active) {
	
	if (active) {	
		m_scanMode = GAP_SCAN_MODE_ACTIVE;
		RPC_DEBUG("setActiveScan ACTIVE...................");
	} else {
		m_scanMode = GAP_SCAN_MODE_PASSIVE;
		RPC_DEBUG("setActiveScan PASSIVE...................");
	}
    
} // setActiveScan

/**
 * @brief Set the interval to scan.
 * @param [in] The interval in msecs.
 */
void BLEScan::setInterval(uint16_t intervalMSecs) {
	if ((intervalMSecs >= 3) && (intervalMSecs <= 10240)) {
        m_scanInterval = (intervalMSecs*1000/625);
    }
} // setInterval


/**
 * @brief Set the window to actively scan.
 * @param [in] windowMSecs How long to actively scan.
 */
void BLEScan::setWindow(uint16_t windowMSecs) {
	if ((windowMSecs * 1000 / 625) > m_scanInterval) {
        return;
    }
    if ((windowMSecs >= 3) && (windowMSecs <= 10240)) {
        m_scanWindow = (windowMSecs*1000/625);
    }	
} // setWindow


/**
 * Set scan parameters
 */
void BLEScan::updateScanParams() {
    uint8_t  _scanMode = m_scanMode;
    le_scan_set_param(GAP_PARAM_SCAN_MODE, sizeof(_scanMode), &_scanMode);
	uint16_t _scanInterval = m_scanInterval;
	le_scan_set_param(GAP_PARAM_SCAN_INTERVAL, sizeof(_scanInterval), &_scanInterval);
    uint16_t _scanWindow = m_scanWindow;
    le_scan_set_param(GAP_PARAM_SCAN_WINDOW, sizeof(_scanWindow), &_scanWindow);
}

/**
 * @brief Start scanning and block until scanning has been completed.
 * @param [in] duration The duration in seconds for which to scan.
 * @return The BLEScanResults.
 */
BLEScanResults BLEScan::start(uint32_t duration, bool is_continue) {
	if (!BLEDevice::ble_start_flags)
	{
		BLEDevice::ble_start_flags = true;
		ble_start();
	}
	if(start(duration, nullptr, is_continue)) {
		m_semaphoreScanEnd.wait("start");   // Wait for the semaphore to release.
	}
	return m_scanResults;
} // start



/**
 * @brief Start scanning.
 * @param [in] duration The duration in seconds for which to scan.
 * @param [in] scanCompleteCB A function to be called when scanning has completed.
 * @param [in] are we continue scan (true) or we want to clear stored devices (false)
 * @return True if scan started or false if there was an error.
 */
bool BLEScan::start(uint32_t duration, void (*scanCompleteCB)(BLEScanResults), bool is_continue) {
	m_semaphoreScanEnd.take(std::string("start"));
	m_scanCompleteCB = scanCompleteCB;                  // Save the callback to be invoked when the scan completes.

	//  if we are connecting to devices that are advertising even after being connected, multiconnecting peripherals
	//  then we should not clear map or we will connect the same device few times
	if(!is_continue) {  
		for(auto _dev : m_scanResults.m_vectorAdvertisedDevices){
			delete _dev.second;
		}
		m_scanResults.m_vectorAdvertisedDevices.clear();
	}
    updateScanParams();	
	uint32_t m_duration = duration * 1000;
	le_scan_timer_start(m_duration);
	RPC_DEBUG("Scan is processing, please end\n\r");
    return true;
} // start


/**
 * @brief Stop an in progress scan.
 * @return N/A.
 */
void BLEScan::stop() {
    le_scan_stop();
    m_semaphoreScanEnd.give();
	RPC_DEBUG("Level  BLEScan stop\n\r");
} // stop

// delete peer device from cache after disconnecting, it is required in case we are connecting to devices with not public address
void BLEScan::erase(BLEAddress address) {
	BLEAdvertisedDevice *advertisedDevice = m_scanResults.m_vectorAdvertisedDevices.find(address.toString())->second;
	m_scanResults.m_vectorAdvertisedDevices.erase(address.toString());
	delete advertisedDevice;
}


/**
 * @brief Set the call backs to be invoked.
 * @param [in] pAdvertisedDeviceCallbacks Call backs to be invoked.
 * @param [in] wantDuplicates  True if we wish to be called back with duplicates.  Default is false.
 */
void BLEScan::setAdvertisedDeviceCallbacks(BLEAdvertisedDeviceCallbacks* pAdvertisedDeviceCallbacks, bool wantDuplicates) {
	m_wantDuplicates = wantDuplicates;
	m_pAdvertisedDeviceCallbacks = pAdvertisedDeviceCallbacks;
} // setAdvertisedDeviceCallbacks


/**
 * @brief Return the count of devices found in the last scan.
 * @return The number of devices found in the last scan.
 */
int BLEScanResults::getCount() {
	return m_vectorAdvertisedDevices.size();
} // getCount

/**
 * @brief Return the specified device at the given index.
 * The index should be between 0 and getCount()-1.
 * @param [in] i The index of the device.
 * @return The device at the specified index.
 */
BLEAdvertisedDevice BLEScanResults::getDevice(uint32_t i) {
	uint32_t x = 0;
	BLEAdvertisedDevice dev = *m_vectorAdvertisedDevices.begin()->second;
	for (auto it = m_vectorAdvertisedDevices.begin(); it != m_vectorAdvertisedDevices.end(); it++) {
		dev = *it->second;
		if (x==i)	break;
		x++;
	}
	return dev;
}

BLEScanResults BLEScan::getResults() {
	return m_scanResults;
}

void BLEScan::clearResults() {
	for(auto _dev : m_scanResults.m_vectorAdvertisedDevices){
		delete _dev.second;
	}
	m_scanResults.m_vectorAdvertisedDevices.clear();
}