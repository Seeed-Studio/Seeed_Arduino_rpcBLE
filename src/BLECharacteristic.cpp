/*
 * BLECharacteristic.cpp
 *
 *  Created on: Jun 22, 2017
 *      Author: kolban
 */
#define TAG "BLECharacteristic"
#include <sstream>
#include <string.h>
#include <iomanip>
#include <stdlib.h>
#include "BLECharacteristic.h"
#include "BLEService.h"
#include "BLEDevice.h"
#include "BLE2902.h"
#include "rpc_unified_log.h"

#define NULL_HANDLE (0xffff)

static BLECharacteristicCallbacks defaultCallback; //null-object-pattern

/**
 * @brief Construct a characteristic
 * @param [in] uuid - UUID (const char*) for the characteristic.
 * @param [in] properties - Properties for the characteristic.
 */
BLECharacteristic::BLECharacteristic(const char *uuid, uint32_t properties) : BLECharacteristic(BLEUUID(uuid), properties)
{
}

/**
 * @brief Construct a characteristic
 * @param [in] uuid - UUID for the characteristic.
 * @param [in] properties - Properties for the characteristic.
 */
BLECharacteristic::BLECharacteristic(BLEUUID uuid, uint32_t properties)
{
	m_bleUUID = uuid;
	m_handle = NULL_HANDLE;
	m_properties = (uint8_t)0;
	m_pCallbacks = &defaultCallback;
	m_permissions = 0;

	if (properties & PROPERTY_READ)
	{
		m_permissions |= GATT_PERM_READ;
	}
	if ((properties & PROPERTY_WRITE) || (properties & PROPERTY_WRITE_NR))
	{
		m_permissions |= GATT_PERM_WRITE;
	}
	if ((properties & PROPERTY_NOTIFY) || (properties & PROPERTY_INDICATE))
	{
		m_permissions |= GATT_PERM_NOTIF_IND;
	}

	setBroadcastProperty((properties & PROPERTY_BROADCAST) != 0);
	setReadProperty((properties & PROPERTY_READ) != 0);
	setWriteProperty((properties & PROPERTY_WRITE) != 0);
	setNotifyProperty((properties & PROPERTY_NOTIFY) != 0);
	setIndicateProperty((properties & PROPERTY_INDICATE) != 0);
	setWriteNoResponseProperty((properties & PROPERTY_WRITE_NR) != 0);
} // BLECharacteristic

/**
 * @brief Destructor.
 */
BLECharacteristic::~BLECharacteristic()
{
	//free(m_attr_value); // Release the storage for the value.
} // ~BLECharacteristic

/**
 * @brief Create a new BLE Characteristic associated with this service.
 * @param [in] uuid - The UUID of the characteristic.
 * @param [in] properties - The properties of the characteristic.
 * @return The new BLE characteristic.
 */
BLEDescriptor *BLECharacteristic::createDescriptor(const char *uuid, uint16_t flags, uint32_t permissions, uint16_t max_len)
{
	return createDescriptor(BLEUUID(uuid), flags, permissions, max_len);
}
/**
 * @brief Create a new BLE Characteristic associated with this service.
 * @param [in] uuid - The UUID of the characteristic.
 * @param [in] properties - The properties of the characteristic.
 * @return The new BLE characteristic.
 */
BLEDescriptor *BLECharacteristic::createDescriptor(BLEUUID uuid, uint16_t flags, uint32_t permissions, uint16_t max_len)
{
	BLEDescriptor *pDescriptor = new BLEDescriptor(uuid, flags, permissions, max_len);
	addDescriptor(pDescriptor);
	return pDescriptor;
} // createCharacteristic

/**
 * @brief Associate a descriptor with this characteristic.
 * @param [in] pDescriptor
 * @return N/A.
 */
void BLECharacteristic::addDescriptor(BLEDescriptor *pDescriptor)
{
	m_descriptorMap.setByUUID(pDescriptor->getUUID(), pDescriptor);
} // addDescriptor

/**
 * @brief Return the BLE Descriptor for the given UUID if associated with this characteristic.
 * @param [in] descriptorUUID The UUID of the descriptor that we wish to retrieve.
 * @return The BLE Descriptor.  If no such descriptor is associated with the characteristic, nullptr is returned.
 */
BLEDescriptor *BLECharacteristic::getDescriptorByUUID(const char *descriptorUUID)
{
	return m_descriptorMap.getByUUID(BLEUUID(descriptorUUID));
} // getDescriptorByUUID

BLEDescriptor *BLECharacteristic::getDescriptorByUUID(BLEUUID descriptorUUID)
{
	return m_descriptorMap.getByUUID(descriptorUUID);
} // getDescriptorByUUID

/**
 * @brief Set the permission to broadcast.
 * A characteristics has properties associated with it which define what it is capable of doing.
 * One of these is the broadcast flag.
 * @param [in] value The flag value of the property.
 * @return N/A 
 */
void BLECharacteristic::setBroadcastProperty(bool value)
{
	if (value)
	{
		m_properties = (uint8_t)(m_properties | GATT_CHAR_PROP_BROADCAST);
	}
	else
	{
		m_properties = (uint8_t)(m_properties & ~GATT_CHAR_PROP_BROADCAST);
	}
} // setBroadcastProperty

/**
 * @brief Set the Read property value.
 * @param [in] value Set to true if we are to allow reads.
 */
void BLECharacteristic::setReadProperty(bool value)
{
	if (value)
	{
		m_properties = (uint8_t)(m_properties | GATT_CHAR_PROP_READ);
	}
	else
	{
		m_properties = (uint8_t)(m_properties & ~GATT_CHAR_PROP_READ);
	}
} // setReadProperty

/**
 * @brief Set the Write property value.
 * @param [in] value Set to true if we are to allow writes.
 */
void BLECharacteristic::setWriteProperty(bool value)
{
	if (value)
	{
		m_properties = (uint8_t)(m_properties | GATT_CHAR_PROP_WRITE);
	}
	else
	{
		m_properties = (uint8_t)(m_properties & ~GATT_CHAR_PROP_WRITE);
	}
} // setWriteProperty

/**
 * @brief Set the Notify property value.
 * @param [in] value Set to true if we are to allow notification messages.
 */
void BLECharacteristic::setNotifyProperty(bool value)
{
	if (value)
	{
		m_properties = (uint8_t)(m_properties | GATT_CHAR_PROP_NOTIFY);
	}
	else
	{
		m_properties = (uint8_t)(m_properties & ~GATT_CHAR_PROP_NOTIFY);
	}
} // setNotifyProperty

/**
 * @brief Set the Indicate property value.
 * @param [in] value Set to true if we are to allow indicate messages.
 */
void BLECharacteristic::setIndicateProperty(bool value)
{
	if (value)
	{
		m_properties = (uint8_t)(m_properties | GATT_CHAR_PROP_INDICATE);
	}
	else
	{
		m_properties = (uint8_t)(m_properties & ~GATT_CHAR_PROP_INDICATE);
	}
} // setIndicateProperty

/**
 * @brief Set the Write No Response property value.
 * @param [in] value Set to true if we are to allow writes with no response.
 */
void BLECharacteristic::setWriteNoResponseProperty(bool value)
{
	if (value)
	{
		m_properties = (uint8_t)(m_properties | GATT_CHAR_PROP_WRITE_NO_RSP);
	}
	else
	{
		m_properties = (uint8_t)(m_properties & ~GATT_CHAR_PROP_WRITE_NO_RSP);
	}
} // setWriteNoResponseProperty

void BLECharacteristic::setAccessPermissions(uint32_t perm)
{
	m_permissions = perm;
}

/**
 * @brief Return a string representation of the characteristic.
 * @return A string representation of the characteristic.
 */
std::string BLECharacteristic::toString()
{
	std::string res = "UUID: " + m_bleUUID.toString() + ", handle : 0x";
	char hex[5];
	snprintf(hex, sizeof(hex), "%04x", m_handle);
	res += hex;
	res += " ";
	if (m_properties & GATT_CHAR_PROP_READ)
		res += "Read ";
	if (m_properties & GATT_CHAR_PROP_WRITE)
		res += "Write ";
	if (m_properties & GATT_CHAR_PROP_WRITE_NO_RSP)
		res += "WriteNoResponse ";
	if (m_properties & GATT_CHAR_PROP_BROADCAST)
		res += "Broadcast ";
	if (m_properties & GATT_CHAR_PROP_NOTIFY)
		res += "Notify ";
	if (m_properties & GATT_CHAR_PROP_INDICATE)
		res += "Indicate ";
	return res;
} // toString

void BLECharacteristic::setValue(double &data64)
{
	double temp = data64;
	setValue((uint8_t *)&temp, 8);
} // setValue

void BLECharacteristic::setValue(float &data32)
{
	float temp = data32;
	setValue((uint8_t *)&temp, 4);
} // setValue

void BLECharacteristic::setValue(int &data32)
{
	uint8_t temp[4];
	temp[0] = data32;
	temp[1] = data32 >> 8;
	temp[2] = data32 >> 16;
	temp[3] = data32 >> 24;
	setValue(temp, 4);
} // setValue

void BLECharacteristic::setValue(uint16_t &data16)
{
	uint8_t temp[2];
	temp[0] = data16;
	temp[1] = data16 >> 8;
	setValue(temp, 2);
} // setValue

void BLECharacteristic::setValue(uint32_t &data32)
{
	uint8_t temp[4];
	temp[0] = data32;
	temp[1] = data32 >> 8;
	temp[2] = data32 >> 16;
	temp[3] = data32 >> 24;
	setValue(temp, 4);
} // setValue

/**
 * @brief Set the value of the characteristic from string data.
 * We set the value of the characteristic from the bytes contained in the
 * string.
 * @param [in] Set the value of the characteristic.
 * @return N/A.
 */
void BLECharacteristic::setValue(std::string value)
{
	m_semaphoreSetValue.take();
	m_value.setValue(value);
	m_semaphoreSetValue.give();
} // setValue

/**
 * @brief Set the value of the characteristic.
 * @param [in] data The data to set for the characteristic.
 * @param [in] length The length of the data in bytes.
 */
void BLECharacteristic::setValue(uint8_t *data, size_t length)
{
	m_semaphoreSetValue.take();
	m_value.setValue(data, length);
	m_semaphoreSetValue.give();
	return;
} // setValue

/**
 * @brief Set the callback handlers for this characteristic.
 * @param [in] pCallbacks An instance of a callbacks structure used to define any callbacks for the characteristic.
 */
void BLECharacteristic::setCallbacks(BLECharacteristicCallbacks *pCallbacks)
{
	if (pCallbacks != nullptr)
	{
		m_pCallbacks = pCallbacks;
	}
	else
	{
		m_pCallbacks = &defaultCallback;
	}
} // setCallbacks

BLECharacteristicCallbacks::~BLECharacteristicCallbacks() {}

/**
 * @brief Get the UUID of the characteristic.
 * @return The UUID of the characteristic.
 */
BLEUUID BLECharacteristic::getUUID()
{
	return m_bleUUID;
} // getUUID

/**
 * @brief Retrieve the current raw data of the characteristic.
 * @return A pointer to storage containing the current characteristic data.
 */
uint8_t *BLECharacteristic::getData()
{
	return m_value.getData();
} // getData

uint8_t BLECharacteristic::getProperties()
{
	return m_properties;
} // getProperties

uint32_t BLECharacteristic::getAccessPermissions()
{
	return m_permissions;
}

BLEService *BLECharacteristic::getService()
{
	return m_pService;
}

uint8_t BLECharacteristic::getHandle()
{
	return m_handle;
}

/**
 * @brief Retrieve the current value of the characteristic.
 * @return A pointer to storage containing the current characteristic value.
 */
std::string BLECharacteristic::getValue()
{
	return m_value.getValue();
} // getValue

/**
 * @brief Send an indication.
 * An indication is a transmission of up to the first 20 bytes of the characteristic value.  An indication
 * will block waiting a positive confirmation from the client.
 * @return N/A
 */
void BLECharacteristic::indicate()
{
	notify(false);
} // indicate

/**
 * @brief Send a notify.
 * A notification is a transmission of up to the first 20 bytes of the characteristic value.  An notification
 * will not block; it is a fire and forget.
 * @return N/A.
 */
void BLECharacteristic::notify(bool is_notification)
{
	RPC_DEBUG(">> notify: length: %d", m_value.getValue().length());

	m_pCallbacks->onNotify(this);   // Invoke the notify callback.

	if (getService()->getServer()->getConnectedCount() == 0) {
		RPC_DEBUG("<< notify: No connected clients.");
		m_pCallbacks->onStatus(this, BLECharacteristicCallbacks::Status::ERROR_NO_CLIENT, 0);
		return;
	}

	// Test to see if we have a 0x2902 descriptor.  If we do, then check to see if notification is enabled
	// and, if not, prevent the notification.

	BLE2902 *p2902 = (BLE2902*)getDescriptorByUUID((uint16_t)0x2902);
	if(is_notification) {
		if (p2902 != nullptr && !p2902->getNotifications()) {
			RPC_DEBUG("<< notifications disabled; ignoring");
			m_pCallbacks->onStatus(this, BLECharacteristicCallbacks::Status::ERROR_NOTIFY_DISABLED, 0);   // Invoke the notify callback.
			return;
		}
	}
	else{
		if (p2902 != nullptr && !p2902->getIndications()) {
			RPC_DEBUG("<< indications disabled; ignoring");
			m_pCallbacks->onStatus(this, BLECharacteristicCallbacks::Status::ERROR_INDICATE_DISABLED, 0);   // Invoke the notify callback.
			return;
		}
	}
	for (auto &myPair : getService()->getServer()->getPeerDevices(false)) {
		uint16_t _mtu = (myPair.second.mtu);
		if (m_value.getValue().length() > _mtu - 3) {
			RPC_DEBUG("- Truncating to %d bytes (maximum notify size)", _mtu - 3);
		}

		size_t length = m_value.getValue().length();
		if(!is_notification) // is indication
			m_semaphoreConfEvt.take("indicate");
			bool errRc = server_send_data(0, getService()->getHandle(), getHandle(), (uint8_t *)m_value.getData(), (uint16_t)length, GATT_PDU_TYPE_ANY);
		if (errRc != true) {
			m_semaphoreConfEvt.give();
			m_pCallbacks->onStatus(this, BLECharacteristicCallbacks::Status::ERROR_GATT, errRc);   // Invoke the notify callback.
			return;
		}
		if(!is_notification){ // is indication
			if(!m_semaphoreConfEvt.timedWait("indicate", indicationTimeout)){
				m_pCallbacks->onStatus(this, BLECharacteristicCallbacks::Status::ERROR_INDICATE_TIMEOUT, 0);   // Invoke the notify callback.
			} else {
				uint32_t code =  m_semaphoreConfEvt.value();
				if(code == 0) {
					m_pCallbacks->onStatus(this, BLECharacteristicCallbacks::Status::SUCCESS_INDICATE, code);   // Invoke the notify callback.
				} else {
					m_pCallbacks->onStatus(this, BLECharacteristicCallbacks::Status::ERROR_INDICATE_FAILURE, code);
				}
			}
		} else {
			m_pCallbacks->onStatus(this, BLECharacteristicCallbacks::Status::SUCCESS_NOTIFY, 0);   // Invoke the notify callback.
		}
	}
	RPC_DEBUG("<< notify");

} // Notify

/**
 * @brief Register a new characteristic with the ESP runtime.
 * @param [in] pService The service with which to associate this characteristic.
 */
void BLECharacteristic::executeCreate(BLEService *pService)
{
	m_pService = pService; // Save the service to which this characteristic belongs.
	ble_char_t CHAR;
	CHAR.uuid_length = getUUID().getNative()->len;
	memcpy(&(CHAR.uuid), &(getUUID().getNative()->uuid), CHAR.uuid_length);
	CHAR.properties = getProperties();
	CHAR.permissions = getAccessPermissions();
	uint8_t char_handle1 = ble_create_char(m_pService->getgiff(), CHAR);
	m_handle = char_handle1;
	BLEDescriptor *pDescriptor = m_descriptorMap.getFirst();
	while (pDescriptor != nullptr)
	{
		pDescriptor->executeCreate(this);
		pDescriptor = m_descriptorMap.getNext();
	} // End while

} // executeCreate

/**
 * Handle a GATT server event.
 */

void BLECharacteristic::handleGATTServerEvent(T_SERVER_ID service_id, void *p_data)
{

	ble_service_cb_data_t *cb_data = (ble_service_cb_data_t *)p_data;
	RPC_DEBUG("ble_gatt_server_callback: service_id: %d getHandle():%d cb_data->attrib_handle:%d\n\r", service_id, getHandle(), cb_data->attrib_handle);
	if (getHandle() == cb_data->attrib_handle)
	{
		RPC_DEBUG("handleGATTServerEvent : %d\n\r",  cb_data->attrib_handle);
		switch (cb_data->event)
		{
		case SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION:
		{
			break;
		}
		case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
		{
			m_pCallbacks->onRead(this);
			uint16_t maxOffset = getService()->getServer()->getPeerMTU(getService()->getServer()->getconnId()) - 1;
			size_t length = m_value.getLength();
			uint8_t *p_value = (uint8_t *)m_value.getData();
			if (length - m_value.getReadOffset() < maxOffset)
			{
				cb_data->cb_data_context.read_data.length = length - m_value.getReadOffset();
				cb_data->cb_data_context.read_data.offset = m_value.getReadOffset();
				//memcpy(cb_data->cb_data_context.read_data.p_value, value.data() + cb_data->cb_data_context.read_data.offset, cb_data->cb_data_context.read_data.length);
				cb_data->cb_data_context.read_data.p_value = (uint8_t *)(p_value);
				m_value.setReadOffset(0);
			}
			else
			{
				cb_data->cb_data_context.read_data.length = maxOffset;
				cb_data->cb_data_context.read_data.offset = m_value.getReadOffset();
				//memcpy(cb_data->cb_data_context.read_data.p_value, value.data() + cb_data->cb_data_context.read_data.offset, cb_data->cb_data_context.read_data.length);
				cb_data->cb_data_context.read_data.p_value = ((uint8_t *)p_value + cb_data->cb_data_context.read_data.offset);
				m_value.setReadOffset(cb_data->cb_data_context.read_data.offset + maxOffset);
			}
			break;
		}
		case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:
		{
			if (getHandle() == cb_data->attrib_handle)
			{
				m_value.addPart(cb_data->cb_data_context.write_data.p_value, cb_data->cb_data_context.write_data.length);
				m_value.commit();
				setValue(cb_data->cb_data_context.write_data.p_value, cb_data->cb_data_context.write_data.length);
				m_pCallbacks->onWrite(this);
				break;
			}
		}
		default:
			break;
		}
	}
	m_descriptorMap.handleGATTServerEvent(service_id, p_data);
} // handleGATTServerEvent

/**
 * @brief Callback function to support a read request.
 * @param [in] pCharacteristic The characteristic that is the source of the event.
 */
void BLECharacteristicCallbacks::onRead(BLECharacteristic *pCharacteristic)
{

} // onRead

/**
 * @brief Callback function to support a write request.
 * @param [in] pCharacteristic The characteristic that is the source of the event.
 */
void BLECharacteristicCallbacks::onWrite(BLECharacteristic *pCharacteristic)
{

} // onWrite

/**
 * @brief Callback function to support a Notify request.
 * @param [in] pCharacteristic The characteristic that is the source of the event.
 */
void BLECharacteristicCallbacks::onNotify(BLECharacteristic *pCharacteristic)
{

} // onNotify

/**
 * @brief Callback function to support a Notify/Indicate Status report.
 * @param [in] pCharacteristic The characteristic that is the source of the event.
 * @param [in] s Status of the notification/indication
 * @param [in] code Additional code of underlying errors
 */
void BLECharacteristicCallbacks::onStatus(BLECharacteristic *pCharacteristic, Status s, uint32_t code)
{

} // onStatus