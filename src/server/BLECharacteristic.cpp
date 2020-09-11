/*
 * BLECharacteristic.cpp
 *
 *  Created on: Jun 22, 2017
 *      Author: kolban
 */
#include <sstream>
#include <string.h>
#include <iomanip>
#include <stdlib.h>
#include "BLECharacteristic.h"
#include "BLEService.h"
#include "BLEDevice.h"

#define NULL_HANDLE (0xffff)

static BLECharacteristicCallbacks defaultCallback; //null-object-pattern

/**
 * @brief Construct a characteristic
 * @param [in] uuid - UUID for the characteristic.
 * @param [in] properties - Properties for the characteristic.
 */
BLECharacteristic::BLECharacteristic(BLEUUID uuid, uint32_t properties) {
	m_bleUUID    = uuid;
	m_handle     = NULL_HANDLE;
	m_properties = (uint8_t)0;
	m_pCallbacks = &defaultCallback;
	m_permissions = GATT_PERM_ALL;

	setBroadcastProperty((properties & PROPERTY_BROADCAST) != 0);
	setReadProperty((properties & PROPERTY_READ) != 0);
	setWriteProperty((properties & PROPERTY_WRITE) != 0);
	setNotifyProperty((properties & PROPERTY_NOTIFY) != 0);
	setIndicateProperty((properties & PROPERTY_INDICATE) != 0);
	setWriteNoResponseProperty((properties & PROPERTY_WRITE_NR) != 0);
} // BLECharacteristic


/**
 * @brief Create a new BLE Characteristic associated with this service.
 * @param [in] uuid - The UUID of the characteristic.
 * @param [in] properties - The properties of the characteristic.
 * @return The new BLE characteristic.
 */
BLEDescriptor* BLECharacteristic::createDescriptor(const char* uuid, uint16_t flags,uint32_t permissions,uint16_t max_len) {
	return createDescriptor(BLEUUID(uuid), flags, permissions, max_len);
}
/**
 * @brief Create a new BLE Characteristic associated with this service.
 * @param [in] uuid - The UUID of the characteristic.
 * @param [in] properties - The properties of the characteristic.
 * @return The new BLE characteristic.
 */
BLEDescriptor* BLECharacteristic::createDescriptor(BLEUUID uuid,uint16_t flags,uint32_t permissions,uint16_t max_len) {
	Serial.printf("createDescriptor start\n\r");
	BLEDescriptor* pDescriptor = new BLEDescriptor(uuid,flags,permissions,max_len);
	addDescriptor(pDescriptor);
	Serial.printf("createDescriptor end\n\r");
	return pDescriptor;
} // createCharacteristic


/**
 * @brief Associate a descriptor with this characteristic.
 * @param [in] pDescriptor
 * @return N/A.
 */
void BLECharacteristic::addDescriptor(BLEDescriptor* pDescriptor) {
	m_descriptorMap.setByUUID(pDescriptor->getUUID(), pDescriptor);
} // addDescriptor


BLEDescriptor* BLECharacteristic::getDescriptorByUUID(BLEUUID descriptorUUID) {
	return m_descriptorMap.getByUUID(descriptorUUID);
} // getDescriptorByUUID


/**
 * @brief Set the permission to broadcast.
 * A characteristics has properties associated with it which define what it is capable of doing.
 * One of these is the broadcast flag.
 * @param [in] value The flag value of the property.
 * @return N/A 
 */
void BLECharacteristic::setBroadcastProperty(bool value) {
#if 1	
	if (value) {
		m_properties = (uint8_t)(m_properties | GATT_CHAR_PROP_BROADCAST);
	} else {
		m_properties = (uint8_t)(m_properties & ~GATT_CHAR_PROP_BROADCAST);
	}
#endif 
} // setBroadcastProperty


/**
 * @brief Set the Read property value.
 * @param [in] value Set to true if we are to allow reads.
 */
void BLECharacteristic::setReadProperty(bool value) {
#if 1
	if (value) {
		m_properties = (uint8_t)(m_properties | GATT_CHAR_PROP_READ);
	} else {
		m_properties = (uint8_t)(m_properties & ~GATT_CHAR_PROP_READ);
	}
#endif 
} // setReadProperty


/**
 * @brief Set the Write property value.
 * @param [in] value Set to true if we are to allow writes.
 */
void BLECharacteristic::setWriteProperty(bool value) {
#if 1
	if (value) {
		m_properties = (uint8_t)(m_properties | GATT_CHAR_PROP_WRITE);
	} else {
		m_properties = (uint8_t)(m_properties & ~GATT_CHAR_PROP_WRITE);
	}
#endif
} // setWriteProperty


/**
 * @brief Set the Notify property value.
 * @param [in] value Set to true if we are to allow notification messages.
 */
void BLECharacteristic::setNotifyProperty(bool value) {
#if 1
	if (value) {
		m_properties = (uint8_t)(m_properties | GATT_CHAR_PROP_NOTIFY);
	} else {
		m_properties = (uint8_t)(m_properties & ~GATT_CHAR_PROP_NOTIFY);
	}
#endif
} // setNotifyProperty


/**
 * @brief Set the Indicate property value.
 * @param [in] value Set to true if we are to allow indicate messages.
 */
void BLECharacteristic::setIndicateProperty(bool value) {
#if 1
	if (value) {
		m_properties = (uint8_t)(m_properties | GATT_CHAR_PROP_INDICATE);
	} else {
		m_properties = (uint8_t)(m_properties & ~GATT_CHAR_PROP_INDICATE);
	}
#endif
} // setIndicateProperty


/**
 * @brief Set the Write No Response property value.
 * @param [in] value Set to true if we are to allow writes with no response.
 */
void BLECharacteristic::setWriteNoResponseProperty(bool value) {
#if 1
	if (value) {
		m_properties = (uint8_t)(m_properties | GATT_CHAR_PROP_WRITE_NO_RSP);
	} else {
		m_properties = (uint8_t)(m_properties & ~GATT_CHAR_PROP_WRITE_NO_RSP);
	}
#endif
} // setWriteNoResponseProperty


void BLECharacteristic::setAccessPermissions(uint32_t perm) {
	m_permissions = perm;
}

/**
 * @brief Set the value of the characteristic from string data.
 * We set the value of the characteristic from the bytes contained in the
 * string.
 * @param [in] Set the value of the characteristic.
 * @return N/A.
 */
void BLECharacteristic::setValue(std::string value) {
	setValue((uint8_t*)(value.data()), value.length());
} // setValue


/**
 * @brief Set the value of the characteristic.
 * @param [in] data The data to set for the characteristic.
 * @param [in] length The length of the data in bytes.
 */
void BLECharacteristic::setValue(uint8_t* data, size_t length) {
	m_semaphoreSetValue.take();  
	m_value.setValue(data, length);
	m_semaphoreSetValue.give();  
} // setValue



/**
 * @brief Set the callback handlers for this characteristic.
 * @param [in] pCallbacks An instance of a callbacks structure used to define any callbacks for the characteristic.
 */
void BLECharacteristic::setCallbacks(BLECharacteristicCallbacks* pCallbacks) {
	if (pCallbacks != nullptr){
		m_pCallbacks = pCallbacks;
	} else {
		m_pCallbacks = &defaultCallback;
	}
} // setCallbacks

BLECharacteristicCallbacks::~BLECharacteristicCallbacks() {}

/**
 * @brief Get the UUID of the characteristic.
 * @return The UUID of the characteristic.
 */
BLEUUID BLECharacteristic::getUUID() {
	return m_bleUUID;
} // getUUID


uint8_t BLECharacteristic::getProperties() {
	return m_properties;
} // getProperties

uint32_t BLECharacteristic::getAccessPermissions() {
	return m_permissions;
}


BLEService* BLECharacteristic::getService() {
	return m_pService;
}

uint8_t BLECharacteristic::getHandle() {
	return m_handle;
}

/**
 * @brief Retrieve the current value of the characteristic.
 * @return A pointer to storage containing the current characteristic value.
 */
std::string BLECharacteristic::getValue() {
	return m_value.getValue();
} // getValue

/**
 * @brief Register a new characteristic with the ESP runtime.
 * @param [in] pService The service with which to associate this characteristic.
 */
void BLECharacteristic::executeCreate(BLEService* pService) {

//	if (m_handle != NULL_HANDLE) {
//		return;
//	}

	m_pService = pService; // Save the service to which this characteristic belongs.

   // setAccessPermissions(GATT_PERM_READ);
//	esp_attr_control_t control;
//	control.auto_rsp = ESP_GATT_RSP_BY_APP;

//	m_semaphoreCreateEvt.take("executeCreate");
#if 0
	esp_err_t errRc = ::esp_ble_gatts_add_char(
		m_pService->getHandle(),
		getUUID().getNative(),
		static_cast<esp_gatt_perm_t>(m_permissions),
		getProperties(),
		nullptr,
		&control); // Whether to auto respond or not.

	if (errRc != ESP_OK) {
		log_e("<< esp_ble_gatts_add_char: rc=%d %s", errRc, GeneralUtils::errorToString(errRc));
		return;
	}
#endif
//	m_semaphoreCreateEvt.wait("executeCreate");
    ble_char_t CHAR;
	CHAR.uuid_length = getUUID().getNative()->len;
	Serial.printf("BLECharacteristic  executeCreate start\n\r");
	memcpy(&(CHAR.uuid), &(getUUID().getNative()->uuid), CHAR.uuid_length);
	Serial.printf("BLECharacteristic  executeCreate end\n\r");
	CHAR.properties = getProperties();
	CHAR.permissions = getAccessPermissions();
	uint8_t char_handle1 = ble_create_char(m_pService->getgiff(), CHAR);
    m_handle = char_handle1;
	Serial.printf("BLECharacteristic  executeCreate end\n\r");
	BLEDescriptor* pDescriptor = m_descriptorMap.getFirst();
	while (pDescriptor != nullptr) {
		pDescriptor->executeCreate(this);
		pDescriptor = m_descriptorMap.getNext();
	} // End while

} // executeCreate


/**
 * Handle a GATT server event.
 */

void BLECharacteristic::handleGATTServerEvent(T_SERVER_ID service_id, void *p_data) {

    ble_service_cb_data_t *cb_data = (ble_service_cb_data_t *)p_data;
    Serial.println("ble_gatt_server_callback\n\r");
    switch (cb_data->event)
    {
    case SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION:
    {
        break;
    }
    case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
    {
		if (getHandle() == cb_data->attrib_handle)
		{
		Serial.println("SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE\n\r");
		m_pCallbacks->onRead(this);
		uint16_t maxOffset =  getService()->getServer()->getPeerMTU(getService()->getServer()->getconnId()) - 1;
		std::string value = m_value.getValue();
		if (value.length() - m_value.getReadOffset() < maxOffset) {
			cb_data->cb_data_context.read_data.length = value.length() - m_value.getReadOffset();
			cb_data->cb_data_context.read_data.offset = m_value.getReadOffset();
			//memcpy(cb_data->cb_data_context.read_data.p_value, value.data() + cb_data->cb_data_context.read_data.offset, cb_data->cb_data_context.read_data.length);
			cb_data->cb_data_context.read_data.p_value = (uint8_t*)(value.data() + cb_data->cb_data_context.read_data.offset);
			m_value.setReadOffset(0);
		}else
		{
			cb_data->cb_data_context.read_data.length = maxOffset;
			cb_data->cb_data_context.read_data.offset = m_value.getReadOffset();
			//memcpy(cb_data->cb_data_context.read_data.p_value, value.data() + cb_data->cb_data_context.read_data.offset, cb_data->cb_data_context.read_data.length);
			cb_data->cb_data_context.read_data.p_value = (uint8_t*)(value.data() + cb_data->cb_data_context.read_data.offset);
			m_value.setReadOffset(cb_data->cb_data_context.read_data.offset + maxOffset);
		}
        if (value.length() + 1 > maxOffset) {
			// Too big for a single shot entry.
			m_value.setReadOffset(maxOffset);
			cb_data->cb_data_context.read_data.length = maxOffset;
			cb_data->cb_data_context.read_data.offset = 0;
			//memcpy(rsp.attr_value.value, value.data(), rsp.attr_value.len);
			cb_data->cb_data_context.read_data.p_value = (uint8_t*)value.data();
		} else {
			// Will fit in a single packet with no callbacks required.
			cb_data->cb_data_context.read_data.length = value.length();
			cb_data->cb_data_context.read_data.offset = 0;
			cb_data->cb_data_context.read_data.p_value = (uint8_t*)value.data();
			//memcpy(rsp.attr_value.value, value.data(), rsp.attr_value.len);
		}
		}
        break;
    }
    case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:
    {
		if(getHandle() == cb_data->attrib_handle) {
		m_value.addPart(cb_data->cb_data_context.write_data.p_value,cb_data->cb_data_context.write_data.length);
		m_value.commit();
		setValue(cb_data->cb_data_context.write_data.p_value, cb_data->cb_data_context.write_data.length);
		m_pCallbacks->onWrite(this);
        break;
		}
    }
    default:
        break;
    }

	m_descriptorMap.handleGATTServerEvent(service_id,p_data);
} // handleGATTServerEvent




/**
 * @brief Callback function to support a read request.
 * @param [in] pCharacteristic The characteristic that is the source of the event.
 */
void BLECharacteristicCallbacks::onRead(BLECharacteristic* pCharacteristic) {

} // onRead


/**
 * @brief Callback function to support a write request.
 * @param [in] pCharacteristic The characteristic that is the source of the event.
 */
void BLECharacteristicCallbacks::onWrite(BLECharacteristic* pCharacteristic) {

} // onWrite

/**
 * @brief Callback function to support a Notify request.
 * @param [in] pCharacteristic The characteristic that is the source of the event.
 */
void BLECharacteristicCallbacks::onNotify(BLECharacteristic* pCharacteristic) {

} // onNotify

/**
 * @brief Callback function to support a Notify/Indicate Status report.
 * @param [in] pCharacteristic The characteristic that is the source of the event.
 * @param [in] s Status of the notification/indication
 * @param [in] code Additional code of underlying errors
 */
void BLECharacteristicCallbacks::onStatus(BLECharacteristic* pCharacteristic, Status s, uint32_t code) {

} // onStatus