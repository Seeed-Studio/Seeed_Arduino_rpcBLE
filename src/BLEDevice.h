/*
 * BLEDevice.h
 *
 *  Created on: Mar 16, 2017
 *      Author: kolban
 */

#ifndef MAIN_BLEDevice_H_
#define MAIN_BLEDevice_H_

#include <map>               // Part of C++ STL
#include <string>
#include "BLEScan.h"
#include "FreeRTOS.h"
#include "BLEClient.h"
#define BTDEBUG 1

class BLEDevice {
public:
    static BLEClient*  createClient();    // Create a new BLE client.
    static BLEScan*    getScan();         // Get the scan object
	static void        init(std::string deviceName);   // Initialize the local BLE environment.
private:
    static BLEClient*	m_pClient;
    static BLEScan*	   _pBLEScan;
	static T_APP_RESULT gapEventHandler(uint8_t cb_type, void *p_cb_data);	
	
};	

#endif /* MAIN_BLEDevice_H_ */
