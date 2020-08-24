#include <map>                 // Part of C++ Standard library
#include <sstream>             // Part of C++ Standard library
#include <iomanip>             // Part of C++ Standard library

#include "BLEDevice.h"
#include <Arduino.h>
#include "Seeed_erpcUnified.h"



/**
 * Singletons for the BLEDevice.
 */

bool       initialized          = false;  
BLEScan*   BLEDevice::_pBLEScan   = nullptr;
BLEClient* BLEDevice::m_pClient = nullptr;
std::map<uint16_t, conn_status_t> BLEDevice::m_connectedClientsMap;


/**
 * @brief Create a new instance of a client.
 * @return A new instance of the client.
 */
/* STATIC */ BLEClient* BLEDevice::createClient() {
	m_pClient = new BLEClient();
	
//**************初始化clinet*********************
// client_init(1);

//***************注册clint回调函数*****************
//   _basClientId = bas_add_client(clientCallbackDefault, max_link_num);

	return m_pClient;
} // createClient

/**
 * @brief Retrieve the Scan object that we use for scanning.
 * @return The scanning object reference.  This is a singleton object.  The caller should not
 * try and release/delete it.
 */
/* STATIC */ BLEScan* BLEDevice::getScan() {
	if (_pBLEScan == nullptr) {
		_pBLEScan = new BLEScan();
	}
	return _pBLEScan;
} // getScan


/**
 * @brief Initialize the %BLE environment.
 * @param deviceName The device name of the device.
 */
/* STATIC */ void BLEDevice::init(std::string deviceName) {
	if(!initialized){
		initialized = true; // Set the initialization flag to ensure we are only initialized once.  
	/*
	 *
	 */
	rpc_ble_init();
	rpc_gap_config_max_le_link_num(3);
	rpc_le_gap_init(3);
	le_register_app_cb(gapEventHandler);
	
	
	rpc_ble_start();
    Serial.printf("BLEDevice::init...................");
	}
	
} // init

/**
 * @brief Handle GAP events.
 */
/* STATIC */ T_APP_RESULT BLEDevice::gapEventHandler(
	  uint8_t cb_type,
	  void *p_cb_data) {
	T_APP_RESULT ret = APP_RESULT_SUCCESS;
#if 0	  
	if (BLEDevice::m_pClient != nullptr) {
		BLEDevice::m_pClient->handleGAPEvent(event, param);
	}
#endif
	if (BLEDevice::_pBLEScan != nullptr) {
		BLEDevice::getScan()->gapCallbackDefault(cb_type,p_cb_data);
	}
	
#if 0
	if(m_bleAdvertising != nullptr) {
		BLEDevice::getAdvertising()->handleGAPEvent(event, param);
	}

	if(m_customGapHandler != nullptr) {
		BLEDevice::m_customGapHandler(event, param);
	}
#endif
	return ret;
} // gapEventHandler


/**
 * @brief Handle GATT client events.
 *
 * Handler for the GATT client events.
 *
 * @param [in] event
 * @param [in] gattc_if
 * @param [in] param
 */
/* STATIC */ void BLEDevice::gattClientEventHandler(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data) {

	for(auto &myPair : BLEDevice::getPeerDevices(true)) {
		conn_status_t conn_status = (conn_status_t)myPair.second;
		if(((BLEClient*)conn_status.peer_device)->getGattcIf() == client_id || ((BLEClient*)conn_status.peer_device)->getGattcIf() == 0xff || client_id == 0xff){
			((BLEClient*)conn_status.peer_device)->clientCallbackDefault(client_id,conn_id,  p_data);
		}
	}
	
#if 0
	if(m_customGattcHandler != nullptr) {
		m_customGattcHandler(event, gattc_if, param);
	}
#endif 

} // gattClientEventHandler

/* multi connect support */
/* requires a little more work */
std::map<uint16_t, conn_status_t> BLEDevice::getPeerDevices(bool _client) {
	return m_connectedClientsMap;
}

void BLEDevice::addPeerDevice(void* peer, bool _client, uint16_t conn_id) {
	conn_status_t status = {
		.peer_device = peer,
		.connected = true,
		.mtu = 23
	};

	m_connectedClientsMap .insert(std::pair<uint16_t, conn_status_t>(conn_id, status));
}

