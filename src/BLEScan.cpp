/*
 * BLEScan.cpp
 *
 *  Created on: Jul 1, 2020
 *      Author: coolc
 */


#include <map>
#include <Arduino.h>
#include "BLEScan.h"

uint8_t BLEScan::_scanProcessing = 0;


/**
 * Constructor
 */

BLEScan::BLEScan() {
    _scanProcessing = 0;	
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
		Serial.printf("setActiveScan ACTIVE...................");
	} else {
		m_scanMode = GAP_SCAN_MODE_PASSIVE;
		Serial.printf("setActiveScan PASSIVE...................");
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
        Serial.printf("Scan window should be less than or equal to scan interval\r\n");
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
    Serial.printf("Set scan parameters...................\r\n");
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
	updateScanParams();	
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
	
    le_scan_start();
	delay(1000);
    return true;
} // start


/**
 * @brief Stop an in progress scan.
 * @return N/A.
 */
void BLEScan::stop() {
    le_scan_stop();
    m_semaphoreScanEnd.give();
	Serial.printf("Level  BLEScan stop\n\r");
} // stop



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

void BLEScan::clearResults() {
	for(auto _dev : m_scanResults.m_vectorAdvertisedDevices){
		delete _dev.second;
	}
	m_scanResults.m_vectorAdvertisedDevices.clear();
}
