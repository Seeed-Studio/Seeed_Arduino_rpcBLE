/*
 * BLERemoteService.cpp
 *
 *  Created on: Jul 8, 2017
 *      Author: kolban
 */


#define TAG "RemoteService"
#include <sstream>
#include "BLERemoteService.h"
#include "rpc_unified_log.h"
#pragma GCC diagnostic warning "-Wunused-but-set-parameter"

BLERemoteService::BLERemoteService(
	uint16_t    att_handle,
	uint16_t    end_group_handle,
    BLEUUID     uuid,
	BLEClient*    pClient
	) {
	m_uuid    = uuid;
	m_haveCharacteristics = false;
	m_startHandle = att_handle;
	m_endHandle = end_group_handle;
	m_pClient = pClient;
}


BLERemoteService::~BLERemoteService() {
}

/**
 * @brief Get the client associated with this service.
 * @return A reference to the client associated with this service.
 */
BLEClient* BLERemoteService::getClient() {
	return m_pClient;
} // getClient

/**
 * @brief Get the client associated with this service.
 * @return A reference to the client associated with this service.
 */
uint16_t BLERemoteService::getEndHandle() {
	return m_endHandle;
} // getClient


uint16_t BLERemoteService::getStartHandle() {
	return m_startHandle;
} // getStartHandle

uint16_t BLERemoteService::getHandle() {
	return getStartHandle();
} // getHandle


BLEUUID BLERemoteService::getUUID() {
	return m_uuid;
}

/**
 * @brief Read the value of a characteristic associated with this service.
 */
std::string BLERemoteService::getValue(BLEUUID characteristicUuid) {
	std::string ret =  getCharacteristic(characteristicUuid)->readValue();
	return ret;
} // readValue

/**
 * @brief Set the value of a characteristic.
 * @param [in] characteristicUuid The characteristic to set.
 * @param [in] value The value to set.
 * @throws BLEUuidNotFound
 */
void BLERemoteService::setValue(BLEUUID characteristicUuid, std::string value) {
	getCharacteristic(characteristicUuid)->writeValue(value);
} // setValue

/**
 * @brief Get the remote characteristic object for the characteristic UUID.
 * @param [in] uuid Remote characteristic uuid.
 * @return Reference to the remote characteristic object.
 * @throws BLEUuidNotFoundException
 */
BLERemoteCharacteristic* BLERemoteService::getCharacteristic(const char* uuid) {
    return getCharacteristic(BLEUUID(uuid));
} // getCharacteristic

/**
 * @brief Get the characteristic object for the UUID.
 * @param [in] uuid Characteristic uuid.
 * @return Reference to the characteristic object.
 * @throws BLEUuidNotFoundException
 */
BLERemoteCharacteristic* BLERemoteService::getCharacteristic(BLEUUID uuid) {
	if (!m_haveCharacteristics) {
		retrieveCharacteristics();
	}
	std::string v = uuid.toString();
	for (auto &myPair : m_characteristicMap) {
		if (myPair.first == v) {
			return myPair.second;
		}
	}
	// throw new BLEUuidNotFoundException();  // <-- we dont want exception here, which will cause app crash, we want to search if any characteristic can be found one after another
	return nullptr;
} // getCharacteristic

/**
 * @brief Retrieve a map of all the characteristics of this service.
 * @return A map of all the characteristics of this service.
 */
std::map<std::string, BLERemoteCharacteristic*>* BLERemoteService::getCharacteristics() {
	// If is possible that we have not read the characteristics associated with the service so do that
	// now.  The request to retrieve the characteristics by calling "retrieveCharacteristics" is a blocking
	// call and does not return until all the characteristics are available.
	if (!m_haveCharacteristics) {
		retrieveCharacteristics();
	}
	return &m_characteristicMap;
} // getCharacteristics

/**
 * @brief Retrieve a map of all the characteristics of this service.
 * @return A map of all the characteristics of this service.
 */
std::map<uint16_t, BLERemoteCharacteristic*>* BLERemoteService::getCharacteristicsByHandle() {
	// If is possible that we have not read the characteristics associated with the service so do that
	// now.  The request to retrieve the characteristics by calling "retrieveCharacteristics" is a blocking
	// call and does not return until all the characteristics are available.
	if (!m_haveCharacteristics) {
		retrieveCharacteristics();
	}
	return &m_characteristicMapByHandle;
} // getCharacteristicsByHandle

/**
 * @brief This function is designed to get characteristics map when we have multiple characteristics with the same UUID
 */
void BLERemoteService::getCharacteristics(std::map<uint16_t, BLERemoteCharacteristic*>* pCharacteristicMap) {
	pCharacteristicMap = &m_characteristicMapByHandle;
}  // Get the characteristics map.


/**
 * @brief Retrieve all the characteristics for this service.
 * This function will not return until we have all the characteristics.
 * @return N/A
 */
void BLERemoteService::retrieveCharacteristics() {
	removeCharacteristics(); // Forget any previous characteristics.
	client_all_char_discovery(m_pClient->getConnId(), m_pClient->getGattcIf(),m_startHandle,m_endHandle);
    m_semaphoregetchaEvt.take("getCharacteristic");	
	m_haveCharacteristics = (m_semaphoregetchaEvt.wait("getCharacteristic") == 0);	

} // getCharacteristics

/**
 * @brief Delete the characteristics in the characteristics map.
 * We maintain a map called m_characteristicsMap that contains pointers to BLERemoteCharacteristic
 * object references.  Since we allocated these in this class, we are also responsible for deleteing
 * them.  This method does just that.
 * @return N/A.
 */
void BLERemoteService::removeCharacteristics() {
	for (auto &myPair : m_characteristicMap) {
	   delete myPair.second;
	   //m_characteristicMap.erase(myPair.first);  // Should be no need to delete as it will be deleted by the clear
	}
	m_characteristicMap.clear();   // Clear the map
	for (auto &myPair : m_characteristicMapByHandle) {
	   delete myPair.second;
	}
	m_characteristicMapByHandle.clear();   // Clear the map
} // removeCharacteristics

/**
 * @brief Create a string representation of this remote service.
 * @return A string representation of this remote service.
 */
std::string BLERemoteService::toString() {
	std::string res = "Service: uuid: " + m_uuid.toString();
	char val[6];
	res += ", start_handle: ";
	snprintf(val, sizeof(val), "%d", m_startHandle);
	res += val;
	snprintf(val, sizeof(val), "%04x", m_startHandle);
	res += " 0x";
	res += val;
	res += ", end_handle: ";
	snprintf(val, sizeof(val), "%d", m_endHandle);
	res += val;
	snprintf(val, sizeof(val), "%04x", m_endHandle);
	res += " 0x";
	res += val;
	for (auto &myPair : m_characteristicMap) {
		res += "\n" + myPair.second->toString();
	   // myPair.second is the value
	}
	return res;
} // toString

T_APP_RESULT BLERemoteService::clientCallbackDefault(
	T_CLIENT_ID client_id, uint8_t conn_id, void *p_data) {
	T_APP_RESULT result = APP_RESULT_SUCCESS;
    T_BLE_CLIENT_CB_DATA *p_ble_client_cb_data = (T_BLE_CLIENT_CB_DATA *)p_data;

    switch (p_ble_client_cb_data->cb_type)
    {
    case BLE_CLIENT_CB_TYPE_DISCOVERY_STATE:
	{
        RPC_DEBUG("discov_state:%d\n\r", p_ble_client_cb_data->cb_content.discov_state.state);
		//give se
		T_DISCOVERY_STATE state = p_ble_client_cb_data->cb_content.discov_state.state;
		switch(state)
		{
			case DISC_STATE_CHAR_DONE:
			{
			BLERemoteService::m_semaphoregetchaEvt.give(0);
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
      
        case DISC_RESULT_SRV_DATA:
        {
            T_GATT_SERVICE_BY_UUID_ELEM *disc_data = (T_GATT_SERVICE_BY_UUID_ELEM *)&(p_ble_client_cb_data->cb_content.discov_result.result.srv_disc_data);
            RPC_DEBUG("start_handle:%d, end handle:%d\n\r", disc_data->att_handle, disc_data->end_group_handle);
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
			uuid,
			this
		    ); 			
            m_characteristicMap.insert(std::pair<std::string, BLERemoteCharacteristic*>(pNewRemoteCharacteristic->getUUID().toString(), pNewRemoteCharacteristic));
		    m_characteristicMapByHandle.insert(std::pair<uint16_t, BLERemoteCharacteristic*>(disc_data->decl_handle, pNewRemoteCharacteristic));					           
            break;
        }
        case DISC_RESULT_CHAR_UUID128:
        {
            T_GATT_CHARACT_ELEM128 *disc_data = (T_GATT_CHARACT_ELEM128 *)&(p_ble_client_cb_data->cb_content.discov_result.result.char_uuid128_disc_data);
			
			BLEUUID uuid = BLEUUID(disc_data->uuid128,16);
			BLERemoteCharacteristic *pNewRemoteCharacteristic = new BLERemoteCharacteristic(
		    disc_data->decl_handle,
			disc_data->properties,
			disc_data->value_handle,
			uuid,
			this
		    ); 
			
            m_characteristicMap.insert(std::pair<std::string, BLERemoteCharacteristic*>(pNewRemoteCharacteristic->getUUID().toString(), pNewRemoteCharacteristic));
		    m_characteristicMapByHandle.insert(std::pair<uint16_t, BLERemoteCharacteristic*>(disc_data->decl_handle, pNewRemoteCharacteristic));           
            break;
        }
        default:
            break;
        }
       
    }
   
    default:
        break;
    }
	// Send the event to each of the characteristics owned by this service.
	for (auto &myPair : m_characteristicMapByHandle) {
	   myPair.second->clientCallbackDefault(client_id,conn_id,p_data);
	}
	return result;
} // gattClientEventHandler





