/*
 * BLEDevice.cpp
 *
 *  Created on: Mar 22, 2017
 *      Author: kolban
 */
#include "BLEClient.h"
#include <iomanip>
#include <string>
#include <sstream>
#include <unordered_set>
#include "BLEDevice.h"
#include "BLEAdvertisedDevice.h"

/*
 * Design
 * ------
 * When we perform a searchService() requests, we are asking the BLE server to return each of the services
 * that it exposes.  For each service, we received an ESP_GATTC_SEARCH_RES_EVT event which contains details
 * of the exposed service including its UUID.
 *
 * The objects we will invent for a BLEClient will be as follows:
 * * BLERemoteService - A model of a remote service.
 * * BLERemoteCharacteristic - A model of a remote characteristic
 * * BLERemoteDescriptor - A model of a remote descriptor.
 *
 * Since there is a hierarchical relationship here, we will have the idea that from a BLERemoteService will own
 * zero or more remote characteristics and a BLERemoteCharacteristic will own zero or more remote BLEDescriptors.
 *
 * We will assume that a BLERemoteService contains a map that maps BLEUUIDs to the set of owned characteristics
 * and that a BLECharacteristic contains a map that maps BLEUUIDs to the set of owned descriptors.
 *
 *
 */
T_SERVER_ID BLEClient::_basClientId = 0;
uint16_t BLEDevice::m_appId = 0; 


BLEClient::BLEClient() {
		m_gattc_if  =  0xff;
} // BLEClient

/**
 * @brief Set the callbacks that will be invoked.
 */
void BLEClient::setClientCallbacks(BLEClientCallbacks* pClientCallbacks) {
	m_pClientCallbacks = pClientCallbacks;
} // setClientCallbacks


/**
 * Add overloaded function to ease connect to peer device with not public address
 */
bool BLEClient::connect(BLEAdvertisedDevice* device) {
	BLEAddress address =  device->getAddress();
	T_GAP_REMOTE_ADDR_TYPE type = device->getAddressType();
	return connect(address, type);
}


/**
 * @brief Connect to the partner (BLE Server).
 * @param [in] address The address of the partner.
 * @return True on success.
 */
bool BLEClient::connect(BLEAddress address, T_GAP_REMOTE_ADDR_TYPE type) {
	
	m_appId = BLEDevice::m_appId++;
	BLEDevice::addPeerDevice(this, true, m_appId);
	m_peerAddress = address;
//连接到client
#if 0
	le_set_conn_param(GAP_CONN_PARAM_1M, &_connReqParam);
    T_GAP_CAUSE result = le_connect(0, destAddr.data(), destAddrType, _localAddrType, scanTimeout);

    if (result == GAP_CAUSE_SUCCESS) {
        if (BTDEBUG) printf("Connect successful to %s\r\n", destAddr.str());
        return true;
    } else {
        if (BTDEBUG) printf("Connect failed\r\n");
        return false;
    }
#endif 

#if 0
    //*******************getConnid****************************
	uint8_t connId = 0;
	uint8_t* btAddr = targetDevice.getAddr().data();
	
    if (le_get_conn_id(btAddr, addrType, &connId)) {
        return connId;
    } else {
        return -1;
    }
#endif 
  //***********************真正实现连接****************
  //  T_GAP_CONN_INFO connInfo;
  //  le_get_conn_info(connId, pConnInfo);
} // connect

uint8_t BLEClient::getGattcIf() {
	return m_gattc_if;
} // getGattcIf


T_APP_RESULT BLEClient::clientCallbackDefault(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data) {
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;
#if 0
    if (client_id == _basClientId) {
        T_BAS_CLIENT_CB_DATA *p_bas_cb_data = (T_BAS_CLIENT_CB_DATA *)p_data;
        switch (p_bas_cb_data->cb_type) {
            
            case BAS_CLIENT_CB_TYPE_DISC_STATE: {
                switch (p_bas_cb_data->cb_content.disc_state) {
                    case DISC_BAS_FAILED: {
                        if (BTDEBUG) printf("Battery service not found on connection %d device\r\n", conn_id);
                        _basServiceAvaliable[conn_id] = 0;
                        break;
                    }
                    case DISC_BAS_DONE: {
                        if (BTDEBUG) printf("Battery service found on connection %d device\r\n", conn_id);
                        _basServiceAvaliable[conn_id] = 1;
                        break;
                    }
                    default:
                        break;
                }
                if (_pDiscoveryCB != nullptr) {
                    _pDiscoveryCB(conn_id, _basServiceAvaliable[conn_id]);
                }
                break;
            }
            
            case BAS_CLIENT_CB_TYPE_READ_RESULT: {
                switch (p_bas_cb_da      ta->cb_content.read_result.type) {
                    case BAS_READ_NOTIFY: {
                        if (BTDEBUG) printf("Read result connection %d device notification state %d\r\n", conn_id, p_bas_cb_data->cb_content.read_result.data.notify);
                        _basNotificationEnabled[conn_id] = p_bas_cb_data->cb_content.read_result.data.notify;
                        break;
                    }
                    case BAS_READ_BATTERY_LEVEL: {
                        if (BTDEBUG) printf("Read result connection %d device battery level %d\r\n", conn_id, p_bas_cb_data->cb_content.read_result.data.battery_level);
                        _battLevel[conn_id] = p_bas_cb_data->cb_content.read_result.data.battery_level;
                        if (_pReadCB != nullptr) {
                            _pReadCB(conn_id, _battLevel[conn_id]);
                        }
                        break;
                    }
                }
                break;
            }
            
            case BAS_CLIENT_CB_TYPE_WRITE_RESULT: {
                switch (p_bas_cb_data->cb_content.write_result.type) {
                    case BAS_WRITE_NOTIFY_ENABLE: {
                        if (BTDEBUG) printf("Write result connection %d device notification enabled\r\n", conn_id);
                        _basNotificationEnabled[conn_id] = 1;
                        break;
                    }
                    case BAS_WRITE_NOTIFY_DISABLE: {
                        if (BTDEBUG) printf("Write result connection %d device notification disabled\r\n", conn_id);
                        _basNotificationEnabled[conn_id] = 0;
                        break;
                    }
                }
                break;
            }
            
            case BAS_CLIENT_CB_TYPE_NOTIF_IND_RESULT: {
                if (BTDEBUG) printf("Received notification connection %d device battery level %d\r\n", conn_id, p_bas_cb_data->cb_content.notify_data.battery_level);
                _battLevel[conn_id] = p_bas_cb_data->cb_content.notify_data.battery_level;
                if (_pNotificationCB != nullptr) {
                    _pNotificationCB(conn_id, _battLevel[conn_id]);
                }
                break;
            }
            
            default:
                if (BTDEBUG) printf("Unhandled callback type\r\n");
                break;
        }
    }
#endif
    return app_result;
 
}
