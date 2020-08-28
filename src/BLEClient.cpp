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


uint16_t BLEDevice::m_appId = 0; 



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
//******************************需要修改**************************************	
	//m_peerAddress = address;
	//uint8_t bd_addr[6] = {0x7d, 0x18, 0x1b, 0xf1, 0xf7, 0x2c};
	
//连接到client
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


  //***********************真正实现连接****************
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
	
	Serial.print("end  BLEClient::getServices():\n\r "); 
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
	Serial.print("start  BLEClient::getServices():\n\r "); 
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


T_APP_RESULT BLEClient::clientCallbackDefault(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data) {
 
	T_APP_RESULT result = APP_RESULT_SUCCESS;
    T_BLE_CLIENT_CB_DATA *p_ble_client_cb_data = (T_BLE_CLIENT_CB_DATA *)p_data;
	
    switch (p_ble_client_cb_data->cb_type)
    {
    case BLE_CLIENT_CB_TYPE_DISCOVERY_STATE:
	{
        Serial.printf("discov_state:%d\n\r", p_ble_client_cb_data->cb_content.discov_state.state);
		//give se
		T_DISCOVERY_STATE state = p_ble_client_cb_data->cb_content.discov_state.state;
		switch(state)
		{
			case DISC_STATE_SRV_DONE:
			{
			m_semaphoreSearchCmplEvt.give(0); 
			Serial.printf("m_semaphoreSearchCmplEvt.give(0)\n\r");
			break;
			}
			case DISC_STATE_CHAR_DONE:
			{
			BLERemoteService::_this->m_semaphoregetchaEvt.give(0);
			Serial.printf("m_semaphoregetchaEvt");
			break;
			}		
            case DISC_STATE_CHAR_DESCRIPTOR_DONE:
			{
			BLERemoteCharacteristic::_this->m_semaphoregetdescEvt.give(0);
			Serial.printf("m_semaphoregetchaEvt");
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
            Serial.printf("discov_type:%d\n\r", discov_type);
        case DISC_RESULT_ALL_SRV_UUID16:
        {   
            T_GATT_SERVICE_ELEM16 *disc_data = (T_GATT_SERVICE_ELEM16 *)&(p_ble_client_cb_data->cb_content.discov_result.result.srv_uuid16_disc_data);
			
			BLEUUID uuid = BLEUUID(disc_data->uuid16);
			BLERemoteService* pRemoteService = new BLERemoteService( 
			disc_data->att_handle,
            disc_data->end_group_handle,
			disc_data->uuid16,
			this
			);
			
			Serial.println(pRemoteService->getUUID().toString().c_str());
			
            Serial.print("m_servicesMap.insert start16:\n\r "); 
			m_servicesMap.insert(std::pair<std::string, BLERemoteService*>(uuid.toString(), pRemoteService));			
            Serial.print("m_servicesMap.insert end16:\n\r ");           			
				
			break;
        }
        case DISC_RESULT_ALL_SRV_UUID128:
        {
            T_GATT_SERVICE_ELEM128 *disc_data = (T_GATT_SERVICE_ELEM128 *)&(p_ble_client_cb_data->cb_content.discov_result.result.srv_uuid128_disc_data);
     
            break;
        }
        case DISC_RESULT_SRV_DATA:
        {
            T_GATT_SERVICE_BY_UUID_ELEM *disc_data = (T_GATT_SERVICE_BY_UUID_ELEM *)&(p_ble_client_cb_data->cb_content.discov_result.result.srv_disc_data);
            Serial.printf("start_handle:%d, end handle:%d\n\r", disc_data->att_handle, disc_data->end_group_handle);
            break;
        }
        case DISC_RESULT_CHAR_UUID16:
        {
            T_GATT_CHARACT_ELEM16 *disc_data = (T_GATT_CHARACT_ELEM16 *)&(p_ble_client_cb_data->cb_content.discov_result.result.char_uuid16_disc_data);
			
			BLEUUID uuid = BLEUUID(disc_data->uuid16);
			BLERemoteCharacteristic *pNewRemoteCharacteristic = new BLERemoteCharacteristic(
		    disc_data->decl_handle,
			disc_data->properties,
			disc_data->value_handle,
			disc_data->uuid16,
			BLERemoteService::_this
		    ); 
			
            m_characteristicMap.insert(std::pair<std::string, BLERemoteCharacteristic*>(pNewRemoteCharacteristic->getUUID().toString(), pNewRemoteCharacteristic));
		    m_characteristicMapByHandle.insert(std::pair<uint16_t, BLERemoteCharacteristic*>(disc_data->decl_handle, pNewRemoteCharacteristic));					
           
            break;
        }
        case DISC_RESULT_CHAR_UUID128:
        {
            T_GATT_CHARACT_ELEM128 *disc_data = (T_GATT_CHARACT_ELEM128 *)&(p_ble_client_cb_data->cb_content.discov_result.result.char_uuid128_disc_data);
           
            break;
        }
        case DISC_RESULT_CHAR_DESC_UUID16:
        {
            T_GATT_CHARACT_DESC_ELEM16 *disc_data = (T_GATT_CHARACT_DESC_ELEM16 *)&(p_ble_client_cb_data->cb_content.discov_result.result.char_desc_uuid16_disc_data);
			
			BLERemoteDescriptor* pNewRemoteDescriptor = new BLERemoteDescriptor(
			disc_data->handle,
			BLEUUID(disc_data->uuid16),
			BLERemoteCharacteristic::_this
		    );  
			Serial.println(pNewRemoteDescriptor->getUUID().toString().c_str());
			Serial.print("m_descriptorMap.insert start\n\r ");
		    m_descriptorMap.insert(std::pair<std::string, BLERemoteDescriptor*>(pNewRemoteDescriptor->getUUID().toString(), pNewRemoteDescriptor));
			Serial.print("m_descriptorMap.insert end\n\r ");
			break;
        }
        case DISC_RESULT_CHAR_DESC_UUID128:
        {
            T_GATT_CHARACT_DESC_ELEM128 *disc_data = (T_GATT_CHARACT_DESC_ELEM128 *)&(p_ble_client_cb_data->cb_content.discov_result.result.char_desc_uuid128_disc_data);
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
    case BLE_CLIENT_CB_TYPE_DISCONNECT_RESULT:
        break;
    default:
        break;
    }
	
	// Pass the request on to all services.
	for (auto &myPair : m_servicesMap) {
	   myPair.second->clientCallbackDefault(client_id,conn_id,p_data);
	}
    return result;
 
}
