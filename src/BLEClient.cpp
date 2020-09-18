/*
 * BLEDevice.cpp
 *
 *  Created on: Mar 22, 2017
 *      Author: kolban
 */
#define TAG "client"
#include "BLEClient.h"
#include <iomanip>
#include <string>
#include <sstream>
#include <unordered_set>
#include "BLEDevice.h"
#include "rpc_unified_log.h"
#include "BLEAdvertisedDevice.h"

uint16_t BLEDevice::m_appId = 0; 
BLEClient::BLEClient() {
	m_pClientCallbacks = nullptr;
	m_conn_id          = 0xff;
	m_gattc_if         = 0xff;
	m_haveServices     = false;
	m_isConnected      = false;  // Initially, we are flagged as not connected.
} // BLEClient


/**
 * @brief Destructor.
 */
BLEClient::~BLEClient() {
	// We may have allocated service references associated with this client.  Before we are finished
	// with the client, we must release resources.
	for (auto &myPair : m_servicesMap) {
	   delete myPair.second;
	}
	m_servicesMap.clear();
} // ~BLEClient


/**
 * @brief Set the callbacks that will be invoked.
 */
void BLEClient::setClientCallbacks(BLEClientCallbacks* pClientCallbacks) {
	m_pClientCallbacks = pClientCallbacks;
} // setClientCallbacks


/**
 * @brief Set the value of a specific characteristic associated with a specific service.
 * @param [in] serviceUUID The service that owns the characteristic.
 * @param [in] characteristicUUID The characteristic whose value we wish to write.
 * @throws BLEUuidNotFound
 */
void BLEClient::setValue(BLEUUID serviceUUID, BLEUUID characteristicUUID, std::string value) {
	getService(serviceUUID)->getCharacteristic(characteristicUUID)->writeValue(value);
} // setValue

/**
 * Add overloaded function to ease connect to peer device with not public address
 */
bool BLEClient::connect(BLEAdvertisedDevice* device) {
/*
 * client initialization
 */
    ble_client_init(BLE_CLIENT_MAX_APPS);
    T_CLIENT_ID client_id = ble_add_client(0, BLE_LE_MAX_LINKS);
	m_gattc_if = client_id;
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

//connect client
    T_GAP_LE_CONN_REQ_PARAM conn_req_param;
	conn_req_param.scan_interval = 0x10;
	conn_req_param.scan_window = 0x10;
	conn_req_param.conn_interval_min = 80;
	conn_req_param.conn_interval_max = 80;
	conn_req_param.conn_latency = 0;
	conn_req_param.supv_tout = 1000;
	conn_req_param.ce_len_min = 2 * (conn_req_param.conn_interval_min - 1);
	conn_req_param.ce_len_max = 2 * (conn_req_param.conn_interval_max - 1);
	le_set_conn_param(GAP_CONN_PARAM_1M, &conn_req_param);

    T_GAP_CAUSE result = le_connect(0, (uint8_t *)address.getNative(), type, GAP_LOCAL_ADDR_LE_PUBLIC, 1000);
	if (result == GAP_CAUSE_SUCCESS)
	{
		m_isConnected = true;
	}
	delay(2000);
	uint8_t conn_id = 0xff;
	le_get_conn_id((uint8_t *)address.getNative(), GAP_REMOTE_ADDR_LE_PUBLIC, &conn_id);
	m_conn_id = conn_id;		
} // connect

uint8_t BLEClient::getGattcIf() {
	return m_gattc_if;
} // getGattcIf

uint16_t BLEClient::getConnId() {
	return m_conn_id;
} // getConnId

/**
 * @brief Retrieve the address of the peer.
 *
 * Returns the Bluetooth device address of the %BLE peer to which this client is connected.
 */
BLEAddress BLEClient::getPeerAddress() {
	return m_peerAddress;
} // getAddress

/**
 * @brief Ask the BLE server for the RSSI value.
 * @return The RSSI value.
 */
int BLEClient::getRssi() {
	if (!isConnected()) {
		RPC_DEBUG("<< getRssi(): Not connected");
		return 0;
	}
	// We make the API call to read the RSSI value which is an asynchronous operation.  We expect to receive
	// an ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT to indicate completion.
	//
	m_semaphoreRssiCmplEvt.take("getRssi");
	le_read_rssi(getConnId());
	int rssiValue = m_semaphoreRssiCmplEvt.wait("getRssi");
	return rssiValue;
} // getRssi

uint16_t BLEClient::getMTU() {
	return m_mtu;
}


uint16_t BLEClient::setMTU(uint16_t mtu_size) {
	m_mtu = mtu_size;
	return m_mtu;
}
/**
 * @brief Get the value of a specific characteristic associated with a specific service.
 * @param [in] serviceUUID The service that owns the characteristic.
 * @param [in] characteristicUUID The characteristic whose value we wish to read.
 * @throws BLEUuidNotFound
 */
std::string BLEClient::getValue(BLEUUID serviceUUID, BLEUUID characteristicUUID) {
	std::string ret = getService(serviceUUID)->getCharacteristic(characteristicUUID)->readValue();
	return ret;
} // getValue


/**
 * @brief Disconnect from the peer.
 * @return N/A.
 */
void BLEClient::disconnect() {
	le_disconnect(getConnId());
} // disconnect

/**
 * @brief Are we connected to a partner?
 * @return True if we are connected and false if we are not connected.
 */
bool BLEClient::isConnected() {
	return m_isConnected;
} // isConnected

/**
 * @brief Get the service BLE Remote Service instance corresponding to the uuid.
 * @param [in] uuid The UUID of the service being sought.
 * @return A reference to the Service or nullptr if don't know about it.
 */
BLERemoteService* BLEClient::getService(const char* uuid) {
    return getService(BLEUUID(uuid));
} // getService

/**
 * @brief Get the service object corresponding to the uuid.
 * @param [in] uuid The UUID of the service being sought.
 * @return A reference to the Service or nullptr if don't know about it.
 * @throws BLEUuidNotFound
 */
BLERemoteService* BLEClient::getService(BLEUUID uuid) {
	if (!m_haveServices) {
		getServices();
	}
	std::string uuidStr = uuid.toString();
	
	for (auto &myPair : m_servicesMap) {
		if (myPair.first == uuidStr) {
			return myPair.second;
		}
	} // End of each of the services.
	
	RPC_DEBUG("end  BLEClient::getServices():\n\r "); 
	return nullptr;
} // getService

/**
 * @brief Ask the remote %BLE server for its services.
 * A %BLE Server exposes a set of services for its partners.  Here we ask the server for its set of
 * services and wait until we have received them all.
 * @return N/A
 */
std::map<std::string, BLERemoteService*>* BLEClient::getServices() {

    clearServices(); // Clear any services that may exist.
	client_all_primary_srv_discovery(getConnId(),getGattcIf());
	RPC_DEBUG("start  BLEClient::getServices():\n\r "); 
	m_semaphoreSearchCmplEvt.take("getServices");
	// If sucessfull, remember that we now have services.
	m_haveServices = (m_semaphoreSearchCmplEvt.wait("getServices") == 0);
    m_haveServices = true;
	
	return &m_servicesMap;
} // getServices

/**
 * @brief Clear any existing services.
 *
 */
void BLEClient::clearServices() {
	// Delete all the services.
	for (auto &myPair : m_servicesMap) {
	   delete myPair.second;
	}
	m_servicesMap.clear();
	m_haveServices = false;
} // clearServices


/**
 * @brief Handle a received GAP event.
 *
 * @param [in] event
 * @param [in] param
 */
T_APP_RESULT BLEClient::handleGAPEvent(uint8_t cb_type, void *p_cb_data) {
	RPC_DEBUG("BLEClient ... handling GAP event!");
	T_APP_RESULT result = APP_RESULT_SUCCESS;
    T_LE_CB_DATA *p_data = (T_LE_CB_DATA *)p_cb_data;
	switch (cb_type) {
		case GAP_MSG_LE_READ_RSSI: {
			m_semaphoreRssiCmplEvt.give((p_data->p_le_read_rssi_rsp->rssi));
			break;
		} // GAP_MSG_LE_READ_RSSI

		default:
			break;
	}
    return result;
} // handleGAPEvent

/**
 * @brief Return a string representation of this client.
 * @return A string representation of this client.
 */
std::string BLEClient::toString() {
	std::string res = "peer address: " + m_peerAddress.toString();
	res += "\nServices:\n";
	for (auto &myPair : m_servicesMap) {
		res += myPair.second->toString() + "\n";
	}
	return res;
} // toString

T_APP_RESULT BLEClient::clientCallbackDefault(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data) {
 
	T_APP_RESULT result = APP_RESULT_SUCCESS;
    T_BLE_CLIENT_CB_DATA *p_ble_client_cb_data = (T_BLE_CLIENT_CB_DATA *)p_data;
	
    switch (p_ble_client_cb_data->cb_type)
    {
    case BLE_CLIENT_CB_TYPE_DISCOVERY_STATE:
	{
        RPC_DEBUG("discov_state:%d\n\r", p_ble_client_cb_data->cb_content.discov_state.state);
		T_DISCOVERY_STATE state = p_ble_client_cb_data->cb_content.discov_state.state;
		switch(state)
		{
			case DISC_STATE_SRV_DONE:
			{
			m_semaphoreSearchCmplEvt.give(0); 
			break;
			}		
		}
		
        break;
	}
    case BLE_CLIENT_CB_TYPE_DISCOVERY_RESULT:
    {
		T_DISCOVERY_RESULT_TYPE discov_type = p_ble_client_cb_data->cb_content.discov_result.discov_type;
        switch (discov_type)
        {
            RPC_DEBUG("discov_type:%d\n\r", discov_type);
        case DISC_RESULT_ALL_SRV_UUID16:
        {   
            T_GATT_SERVICE_ELEM16 *disc_data = (T_GATT_SERVICE_ELEM16 *)&(p_ble_client_cb_data->cb_content.discov_result.result.srv_uuid16_disc_data);
			
			BLEUUID uuid = BLEUUID(disc_data->uuid16);
			BLERemoteService* pRemoteService = new BLERemoteService( 
			disc_data->att_handle,
            disc_data->end_group_handle,
			uuid,
			this
			);
			
			RPC_DEBUG(pRemoteService->getUUID().toString().c_str());
			m_servicesMap.insert(std::pair<std::string, BLERemoteService*>(uuid.toString(), pRemoteService));			         				
			break;
        }
        case DISC_RESULT_ALL_SRV_UUID128:
        {
            T_GATT_SERVICE_ELEM128 *disc_data = (T_GATT_SERVICE_ELEM128 *)&(p_ble_client_cb_data->cb_content.discov_result.result.srv_uuid128_disc_data);
			BLEUUID uuid = BLEUUID(disc_data->uuid128,16);
			BLERemoteService* pRemoteService = new BLERemoteService( 
			disc_data->att_handle,
            disc_data->end_group_handle,
			uuid,
			this
			);
			
			RPC_DEBUG(pRemoteService->getUUID().toString().c_str());  
			m_servicesMap.insert(std::pair<std::string, BLERemoteService*>(uuid.toString(), pRemoteService));			
            break;
        }
        case DISC_RESULT_SRV_DATA:
        {
            T_GATT_SERVICE_BY_UUID_ELEM *disc_data = (T_GATT_SERVICE_BY_UUID_ELEM *)&(p_ble_client_cb_data->cb_content.discov_result.result.srv_disc_data);
            RPC_DEBUG("start_handle:%d, end handle:%d\n\r", disc_data->att_handle, disc_data->end_group_handle);
            break;
        }  
     
        default:
            break;
        }
       
    }
    case BLE_CLIENT_CB_TYPE_READ_RESULT:
        break;
    case BLE_CLIENT_CB_TYPE_WRITE_RESULT:
        break;
    case BLE_CLIENT_CB_TYPE_NOTIF_IND:
        break;
    case BLE_CLIENT_CB_TYPE_DISCONNECT_RESULT: {
	    m_isConnected = false;
		if (m_pClientCallbacks != nullptr) {
			m_pClientCallbacks->onDisconnect(this);
		}
		m_semaphoreSearchCmplEvt.give(1);
		BLEDevice::removePeerDevice(m_appId, true);
        break;
	}
    default:
        break;
    }
	
	// Pass the request on to all services.
	for (auto &myPair : m_servicesMap) {
	   myPair.second->clientCallbackDefault(client_id,conn_id,p_data);
	}
    return result;
 
}