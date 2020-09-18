/*
 * BLERemoteCharacteristic.cpp
 *
 *  Created on: Jul 8, 2017
 *      Author: kolban
 */
#define TAG "RemoteCharacteristic"
#include "BLERemoteCharacteristic.h"
#include <sstream>
#include "rpc_unified_log.h"

BLERemoteCharacteristic::BLERemoteCharacteristic(
    uint16_t    decl_handle,  
    uint16_t    properties,   
    uint16_t    value_handle,  
    BLEUUID     uuid,
	BLERemoteService*    pRemoteService
    ) {
	m_handle         = value_handle;
	m_charProp       = properties;
	m_uuid           = uuid;
	m_end_handle     = pRemoteService->getEndHandle();
	m_pRemoteService = pRemoteService;
	m_notifyCallback = nullptr;
	m_rawData = nullptr;
} // BLERemoteCharacteristic

/**
 *@brief Destructor.
 */
BLERemoteCharacteristic::~BLERemoteCharacteristic() {
	removeDescriptors();   // Release resources for any descriptor information we may have allocated.
} // ~BLERemoteCharacteristic

/**
 * @brief Get the UUID for this characteristic.
 * @return The UUID for this characteristic.
 */
BLEUUID BLERemoteCharacteristic::getUUID() {
	return m_uuid;
} // getUUID

/**
 * @brief Get the handle for this characteristic.
 * @return The handle for this characteristic.
 */
uint16_t BLERemoteCharacteristic::getHandle() {
	return m_handle;
} // getHandle

uint16_t BLERemoteCharacteristic::getendHandle() {
	return m_end_handle;
} // getHandle


/**
 * @brief Get the remote service associated with this characteristic.
 * @return The remote service associated with this characteristic.
 */
BLERemoteService* BLERemoteCharacteristic::getRemoteService() {
	return m_pRemoteService;
} // getRemoteService


/**
 * @brief Does the characteristic support reading?
 * @return True if the characteristic supports reading.
 */
bool BLERemoteCharacteristic::canRead() {
	return (m_charProp & GATT_CHAR_PROP_READ) != 0;	
} // canRead


/**
 * @brief Does the characteristic support notifications?
 * @return True if the characteristic supports notifications.
 */
bool BLERemoteCharacteristic::canNotify() {
	return (m_charProp & GATT_CHAR_PROP_NOTIFY) != 0;
} // canNotify


/**
 * @brief Does the characteristic support writing?
 * @return True if the characteristic supports writing.
 */
bool BLERemoteCharacteristic::canWrite() {
	return (m_charProp & GATT_CHAR_PROP_WRITE) != 0;
} // canWrite

/**
 * @brief Does the characteristic support broadcasting?
 * @return True if the characteristic supports broadcasting.
 */
bool BLERemoteCharacteristic::canBroadcast() {
	return (m_charProp & GATT_CHAR_PROP_BROADCAST) != 0;
} // canBroadcast

/**
 * @brief Does the characteristic support indications?
 * @return True if the characteristic supports indications.
 */
bool BLERemoteCharacteristic::canIndicate() {
	return (m_charProp & GATT_CHAR_PROP_INDICATE) != 0;
} // canIndicate

/**
 * @brief Does the characteristic support writing with no response?
 * @return True if the characteristic supports writing with no response.
 */
bool BLERemoteCharacteristic::canWriteNoResponse() {
	return (m_charProp & GATT_CHAR_PROP_WRITE_NO_RSP) != 0;
} // canWriteNoResponse

/**
 * @brief Read the value of the remote characteristic.
 * @return The value of the remote characteristic.
 */
std::string BLERemoteCharacteristic::readValue() {
	// Check to see that we are connected.
	if (!getRemoteService()->getClient()->isConnected()) {
		return std::string();
	}
	m_semaphoreReadCharEvt.take("readValue");
    client_attr_read(m_pRemoteService->getClient()->getConnId(), m_pRemoteService->getClient()->getGattcIf(),getHandle());
	
	// Block waiting for the event that indicates that the read has completed.  When it has, the std::string found
	// in m_value will contain our data.
	m_semaphoreReadCharEvt.wait("readValue");
	return m_value;
} // readValue

/**
 * @brief Read a byte value
 * @return The value as a byte
 */
uint8_t BLERemoteCharacteristic::readUInt8() {
	std::string value = readValue();
	if (value.length() >= 1) {
		return (uint8_t)value[0];
	}
	return 0;
} // readUInt8

/**
 * @brief Read an unsigned 16 bit value
 * @return The unsigned 16 bit value.
 */
uint16_t BLERemoteCharacteristic::readUInt16() {
	std::string value = readValue();
	if (value.length() >= 2) {
		return *(uint16_t*)(value.data());
	}
	return 0;
} // readUInt16

/**
 * @brief Read an unsigned 32 bit value.
 * @return the unsigned 32 bit value.
 */
uint32_t BLERemoteCharacteristic::readUInt32() {
	std::string value = readValue();
	if (value.length() >= 4) {
		return *(uint32_t*)(value.data());
	}
	return 0;
} // readUInt32

/**
 * @brief Read a float value.
 * @return the float value.
 */
float BLERemoteCharacteristic::readFloat() {
	std::string value = readValue();
	if (value.length() >= 4) {
		return *(float*)(value.data());
	}
	return 0.0;
} // readFloat

/**
 * @brief Read raw data from remote characteristic as hex bytes
 * @return return pointer data read
 */
uint8_t* BLERemoteCharacteristic::readRawData() {
	return m_rawData;
}

/**
 * @brief Register for notifications.
 * @param [in] notifyCallback A callback to be invoked for a notification.  If NULL is provided then we are
 * unregistering a notification.
 * @return N/A.
 */
void BLERemoteCharacteristic::registerForNotify(notify_callback notifyCallback, bool notifications) {
	
	m_notifyCallback = notifyCallback;   // Save the notification callback.
	m_semaphoreRegForNotifyEvt.take("registerForNotify");
	if (notifyCallback != nullptr) {   // If we have a callback function, then this is a registration.
		
		uint8_t val[] = {0x01, 0x00};
		if(!notifications) val[0] = 0x02;
		BLERemoteDescriptor* desc = getDescriptor(BLEUUID((uint16_t)0x2902));	
		desc->writeValue(val, 2);
	} // End Register
	else {   // If we weren't passed a callback function, then this is an unregistration.		
		uint8_t val[] = {0x00, 0x00};
		BLERemoteDescriptor* desc = getDescriptor((uint16_t)0x2902);
		desc->writeValue(val, 2);
	} // End Unregister
	m_semaphoreRegForNotifyEvt.wait("registerForNotify");
} // registerForNotify


/**
 * @brief Get the descriptor instance with the given UUID that belongs to this characteristic.
 * @param [in] uuid The UUID of the descriptor to find.
 * @return The Remote descriptor (if present) or null if not present.
 */
BLERemoteDescriptor* BLERemoteCharacteristic::getDescriptor(BLEUUID uuid) {
	std::string v = uuid.toString();
	retrieveDescriptors();
	for (auto &myPair : m_descriptorMap) {
		if (myPair.first == v) {			
			return myPair.second;
		}
	}
	return nullptr;
} // getDescriptor

/**
 * @brief Retrieve the map of descriptors keyed by UUID.
 */
std::map<std::string, BLERemoteDescriptor*>* BLERemoteCharacteristic::getDescriptors() {
	return &m_descriptorMap;
} // getDescriptors

void BLERemoteCharacteristic::writeValue(uint8_t newValue, bool response) {
	writeValue(&newValue, 1, response);
} // writeValue


/**
 * @brief Write the new value for the characteristic.
 * @param [in] newValue The new value to write.
 * @param [in] response Do we expect a response?
 * @return N/A.
 */
void BLERemoteCharacteristic::writeValue(std::string newValue, bool response) {
	writeValue((uint8_t*)newValue.c_str(), strlen(newValue.c_str()), response);
} // writeValue


void BLERemoteCharacteristic::writeValue(uint8_t* data, size_t length, bool response) {
	// Check to see that we are connected.
	if (!getRemoteService()->getClient()->isConnected()) {
		return;
	}
	m_semaphoreWriteCharEvt.take("writeValue");
	// Invoke the ESP-IDF API to perform the write.
	client_attr_write(m_pRemoteService->getClient()->getConnId(),m_pRemoteService->getClient()->getGattcIf(),GATT_WRITE_TYPE_REQ,getHandle(),length,(uint8_t *)data);
	m_semaphoreWriteCharEvt.wait("writeValue");
} // writeValue



/**
 * @brief Convert a BLERemoteCharacteristic to a string representation;
 * @return a String representation.
 */
std::string BLERemoteCharacteristic::toString() {
	std::string res = "Characteristic: uuid: " + m_uuid.toString();
	char val[6];
	res += ", handle: ";
	snprintf(val, sizeof(val), "%d", getHandle());
	res += val;
	res += " 0x";
	snprintf(val, sizeof(val), "%04x", getHandle());
	res += val;
//	res += ", props: " + BLEUtils::characteristicPropertiesToString(m_charProp);
	return res;
} // toString

/**
 * @brief Populate the descriptors (if any) for this characteristic.
 */
void BLERemoteCharacteristic::retrieveDescriptors() {

	removeDescriptors();   // Remove any existing descriptors.
	client_all_char_descriptor_discovery(getRemoteService()->getClient()->getConnId(),getRemoteService()->getClient()->getGattcIf(),
                                                m_handle,m_end_handle);

    m_semaphoregetdescEvt.take("getDescriptor");
	m_haveDescriptor = (m_semaphoregetdescEvt.wait("getDescriptor") == 0);
} // getDescriptors

/**
 * @brief Delete the descriptors in the descriptor map.
 * We maintain a map called m_descriptorMap that contains pointers to BLERemoteDescriptors
 * object references.  Since we allocated these in this class, we are also responsible for deleteing
 * them.  This method does just that.
 * @return N/A.
 */
void BLERemoteCharacteristic::removeDescriptors() {
	// Iterate through all the descriptors releasing their storage and erasing them from the map.
	for (auto &myPair : m_descriptorMap) {
	   m_descriptorMap.erase(myPair.first);
	   delete myPair.second;
	}
	m_descriptorMap.clear();   // Technically not neeeded, but just to be sure.
} // removeCharacteristics

T_APP_RESULT BLERemoteCharacteristic::clientCallbackDefault(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data) {
 
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
            case DISC_STATE_CHAR_DESCRIPTOR_DONE:
			{
			BLERemoteCharacteristic::m_semaphoregetdescEvt.give(0);
			break;
			}
			default:
			    break;					
		}
        break;
	}
    case BLE_CLIENT_CB_TYPE_DISCOVERY_RESULT:
    {   
		T_DISCOVERY_RESULT_TYPE discov_type = p_ble_client_cb_data->cb_content.discov_result.discov_type;
		switch (discov_type)
        {
            RPC_DEBUG("discov_type:%d\n\r", discov_type);
        case DISC_RESULT_CHAR_DESC_UUID16:
        {
            T_GATT_CHARACT_DESC_ELEM16 *disc_data = (T_GATT_CHARACT_DESC_ELEM16 *)&(p_ble_client_cb_data->cb_content.discov_result.result.char_desc_uuid16_disc_data);			
			BLERemoteDescriptor* pNewRemoteDescriptor = new BLERemoteDescriptor(
			disc_data->handle,
			BLEUUID(disc_data->uuid16),
			this
		    );  
			RPC_DEBUG(pNewRemoteDescriptor->getUUID().toString().c_str());
		    m_descriptorMap.insert(std::pair<std::string, BLERemoteDescriptor*>(pNewRemoteDescriptor->getUUID().toString(), pNewRemoteDescriptor));
			break;
        }
        case DISC_RESULT_CHAR_DESC_UUID128:
        {
            T_GATT_CHARACT_DESC_ELEM128 *disc_data = (T_GATT_CHARACT_DESC_ELEM128 *)&(p_ble_client_cb_data->cb_content.discov_result.result.char_desc_uuid128_disc_data);
            BLERemoteDescriptor* pNewRemoteDescriptor = new BLERemoteDescriptor(
			disc_data->handle,
			BLEUUID(disc_data->uuid128,16),
			this
		    );  
			RPC_DEBUG(pNewRemoteDescriptor->getUUID().toString().c_str());
		    m_descriptorMap.insert(std::pair<std::string, BLERemoteDescriptor*>(pNewRemoteDescriptor->getUUID().toString(), pNewRemoteDescriptor));
			break;
        }
        default:
            break;
        }
        break;	
    }
    case BLE_CLIENT_CB_TYPE_READ_RESULT:
	{   
	    m_value = std::string((char*) p_ble_client_cb_data->cb_content.read_result.p_value,p_ble_client_cb_data->cb_content.read_result.value_size);
		if(m_rawData != nullptr) free(m_rawData);
		m_rawData = (uint8_t*) calloc(p_ble_client_cb_data->cb_content.read_result.value_size, sizeof(uint8_t));
	    memcpy(m_rawData, p_ble_client_cb_data->cb_content.read_result.p_value , p_ble_client_cb_data->cb_content.read_result.value_size);
		
	    m_semaphoreReadCharEvt.give();
        break;
	}
    case BLE_CLIENT_CB_TYPE_WRITE_RESULT:
	{
		m_semaphoreWriteCharEvt.give();
        break;
	}
    case BLE_CLIENT_CB_TYPE_NOTIF_IND:{
		if (p_ble_client_cb_data->cb_content.notif_ind.handle != getHandle()) break;
		if (m_notifyCallback != nullptr) {
			m_notifyCallback(this,p_ble_client_cb_data->cb_content.notif_ind.p_value, p_ble_client_cb_data->cb_content.notif_ind.value_size,p_ble_client_cb_data->cb_content.notif_ind.notify);
		} // End we have a callback function ...
		m_semaphoreRegForNotifyEvt.give();
		break;
	}
	
        break;
    case BLE_CLIENT_CB_TYPE_DISCONNECT_RESULT:
        break;
    default:
        break;
    }
	return result;
}


