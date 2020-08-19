/*
 * BLEScan.cpp
 *
 *  Created on: Jul 1, 2017
 *      Author: kolban
 */


#include <map>
#include <Arduino.h>
#include "BLEAdvertisedDevice.h"
#include "BLEScan.h"
#include "Seeed_erpcUnified.h"

uint8_t BLEScan::_scanProcessing = 0;

void beginCentral(uint8_t connCount);
void start_Scan(uint32_t scanDuration_ms);
void setupGAPBondManager();
void start_Scan_1();
void stopScan();



/**
 * Constructor
 */

BLEScan::BLEScan() {
    _scanProcessing = 0;	
} // BLEScan


/**
 *
 *
 *
 */
void updateScanParams() {
    Serial.printf("设置扫描参数...................");
}

/**
 * @brief Start scanning and block until scanning has been completed.
 * @param [in] duration The duration in seconds for which to scan.
 * @return The BLEScanResults.
 */
BLEScanResults BLEScan::start(uint32_t duration, bool is_continue) {
//	updateScanParams();	
	if(start(duration, nullptr, is_continue)) {
//		m_semaphoreScanEnd.wait("start");   // Wait for the semaphore to release.
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
//	m_semaphoreScanEnd.take(std::string("start"));
	m_scanCompleteCB = scanCompleteCB;                  // Save the callback to be invoked when the scan completes.

	//  if we are connecting to devices that are advertising even after being connected, multiconnecting peripherals
	//  then we should not clear map or we will connect the same device few times
	if(!is_continue) {  
		for(auto _dev : m_scanResults.m_vectorAdvertisedDevices){
			delete _dev.second;
		}
		m_scanResults.m_vectorAdvertisedDevices.clear();
	}
//    beginCentral(0);
    uint32_t scanDuration_ms =  duration * 1000;

    start_Scan(scanDuration_ms);
	return true;
} // start

void start_Scan(uint32_t scanDuration_ms) {
#if 1	
    start_Scan_1();
    vTaskDelay(scanDuration_ms / portTICK_RATE_MS);
    stopScan();
    vTaskDelay(100 / portTICK_RATE_MS);
#endif
}

void start_Scan_1() {
    T_GAP_CAUSE cause;
    if (BLEScan::_scanProcessing) {
        Serial.printf("Scan is processing, please stop it first\n\r");
    } else {
        BLEScan::_scanProcessing = 1;
		//启动扫描
        cause = le_scan_start();
        if (cause != GAP_CAUSE_SUCCESS) {
            printf("Scan error\n\r");
            BLEScan::_scanProcessing = 0;
       }
    }	
}

void stopScan() {
    if (BLEScan::_scanProcessing) {
		//停止扫描
//        le_scan_stop();
        le_scan_stop();
        BLEScan::_scanProcessing = 0;
    } else {
       Serial.printf("There is no scan\n\r");
    }
}


/**
 * @brief Should we perform an active or passive scan?
 * The default is a passive scan.  An active scan means that we will wish a scan response.
 * @param [in] active If true, we perform an active scan otherwise a passive scan.
 * @return N/A.
 */
void BLEScan::setActiveScan(bool active) {
	
	if (active) {
		
		//_scanMode = GAP_SCAN_MODE_ACTIVE;
		Serial.printf("setActiveScan ACTIVE...................");
	} else {
		//_scanMode = GAP_SCAN_MODE_PASSIVE;
		Serial.printf("setActiveScan PASSIVE...................");
	}
    
} // setActiveScan

/**
 * @brief Set the interval to scan.
 * @param [in] The interval in msecs.
 */
void BLEScan::setInterval(uint16_t intervalMSecs) {
	if ((intervalMSecs >= 3) && (intervalMSecs <= 10240)) {
        _scanInterval = (intervalMSecs*1000/625);
    }
} // setInterval


/**
 * @brief Set the window to actively scan.
 * @param [in] windowMSecs How long to actively scan.
 */
void BLEScan::setWindow(uint16_t windowMSecs) {
	if ((windowMSecs * 1000 / 625) > _scanInterval) {
        Serial.printf("Scan window should be less than or equal to scan interval\r\n");
        return;
    }
    if ((windowMSecs >= 3) && (windowMSecs <= 10240)) {
        _scanWindow = (windowMSecs*1000/625);
    }	
} // setWindow



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
 *starts the BLE stack to operate as a central device
 *note: central devices should not advertise to other devices
 *
 */
static uint8_t _bleState;  // 0 = not running, 1 = peripheral, 2 = central
void beginCentral(uint8_t connCount) {
	//状态标志
    //T_GAP_DEV_STATE new_state;
    if (_bleState != 0) {
        Serial.printf("BLE already running, unable to start central\r\n");
        return;
    } else {
        _bleState = 2;
    }
    //// Config APP LE link number
	Serial.printf("Config APP LE link number");
#if 0
    if (connCount <= BLE_CENTRAL_APP_MAX_LINKS) {
        gap_config_max_le_link_num(connCount);
        le_gap_init(connCount);
    } else {
        printf("Recommended max link count exceeded\r\n");
    }
#endif

#if 0
    ///set  phy
    uint8_t  phys_prefer = GAP_PHYS_PREFER_ALL;
    uint8_t  tx_phys_prefer = GAP_PHYS_PREFER_1M_BIT | GAP_PHYS_PREFER_2M_BIT | GAP_PHYS_PREFER_CODED_BIT;
    uint8_t  rx_phys_prefer = GAP_PHYS_PREFER_1M_BIT | GAP_PHYS_PREFER_2M_BIT | GAP_PHYS_PREFER_CODED_BIT;
    le_set_gap_param(GAP_PARAM_DEFAULT_PHYS_PREFER, sizeof(phys_prefer), &phys_prefer);
    le_set_gap_param(GAP_PARAM_DEFAULT_TX_PHYS_PREFER, sizeof(tx_phys_prefer), &tx_phys_prefer);
    le_set_gap_param(GAP_PARAM_DEFAULT_RX_PHYS_PREFER, sizeof(rx_phys_prefer), &rx_phys_prefer);
#endif 

#if 0
    // update device parameters
    le_set_gap_param(GAP_PARAM_DEVICE_NAME, GAP_DEVICE_NAME_LEN, _deviceName);
    le_set_gap_param(GAP_PARAM_APPEARANCE, sizeof(_appearance), &_appearance);
#endif

#if 0
    // update scan parameters
    updateScanParams();
    if (BTDEBUG) printf("Scan update\r\n");
#endif

    setupGAPBondManager();

    // register callback to handle app GAP message
#if 0	
    le_register_app_cb(gapCallbackDefault);
    if (BTDEBUG) printf("GAP cb reg\r\n");
#endif

#if 0
    // register clients and callbacks
    client_register_general_client_cb(appClientCallbackDefault);
    //ble_central_gcs_client_id = gcs_add_client(ble_central_gcs_client_callback, BLE_CENTRAL_APP_MAX_LINKS, BLE_CENTRAL_APP_MAX_DISCOV_TABLE_NUM);
#endif

#if 0
    // start BLE main task to handle IO and GAP msg
    os_task_create(&_appTaskHandle, "BLE_Central_Task", BLEMainTask, 0, 256*6, 1);
    if (BTDEBUG) printf("Task create\r\n");
#endif

#if 0
    bt_coex_init();
    if (BTDEBUG) printf("Coex init\r\n");
#endif

#if 0
    /*Wait BT init complete*/
    do {
        vTaskDelay(100 / portTICK_RATE_MS);
        le_get_gap_param(GAP_PARAM_DEV_STATE , &new_state);
    }while(new_state.gap_init_state != GAP_INIT_STATE_STACK_READY);

    /*Start BT WIFI coexistence*/
    wifi_btcoex_set_bt_on();
    if (BTDEBUG) printf("Coex on\r\n");
#endif	
}

void setupGAPBondManager() {
    // Setup the GAP Bond Manager
	Serial.printf("Setup the GAP Bond Manager");
#if 0	
    gap_set_param(GAP_PARAM_BOND_PAIRING_MODE, sizeof(_authPairMode), &_authPairMode);
    gap_set_param(GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, sizeof(_authFlags), &_authFlags);
    gap_set_param(GAP_PARAM_BOND_IO_CAPABILITIES, sizeof(_authIoCap), &_authIoCap);
    gap_set_param(GAP_PARAM_BOND_OOB_ENABLED, sizeof(_authOob), &_authOob);
    le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY, sizeof(_authFixPasskey), &_authFixPasskey);
    le_bond_set_param(GAP_PARAM_BOND_FIXED_PASSKEY_ENABLE, sizeof(_authUseFixPasskey), &_authUseFixPasskey);
    le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_ENABLE, sizeof(_authSecReqEnable), &_authSecReqEnable);
    le_bond_set_param(GAP_PARAM_BOND_SEC_REQ_REQUIREMENT, sizeof(_authSecReqFlags), &_authSecReqFlags);
#endif
}


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
