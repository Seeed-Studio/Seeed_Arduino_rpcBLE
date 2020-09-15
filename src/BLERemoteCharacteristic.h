
#ifndef COMPONENTS_CPP_UTILS_BLEREMOTECHARACTERISTIC_H_
#define COMPONENTS_CPP_UTILS_BLEREMOTECHARACTERISTIC_H_
#include <string>
#include "BLERemoteService.h"
#include "BLEUUID.h"
#include "BLEFreeRTOS.h"
#include "BLERemoteDescriptor.h"
#include "Seeed_erpcUnified.h"

class BLERemoteService;
class BLERemoteDescriptor;
class BLERemoteCharacteristic;

typedef void (*notify_callback)(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify);
/**
 * @brief A model of a remote %BLE characteristic.
 */
class BLERemoteCharacteristic {
public:
	BLEUUID     getUUID();
    uint16_t    getHandle();
	uint16_t     getendHandle();
	
	bool        canRead();
	bool        canNotify();
	bool        canWrite();
	
	std::string readValue();
	
	void        writeValue(uint8_t* data, size_t length, bool response = false);
	void        writeValue(std::string newValue, bool response = false);
	void        writeValue(uint8_t newValue, bool response = false);
	
	BLERemoteDescriptor* getDescriptor(BLEUUID uuid);
	void        registerForNotify(notify_callback _callback, bool notifications = true);
	
	std::string toString();
	
private:
    friend class BLEClient;
	friend class BLERemoteService;
	friend class BLERemoteDescriptor; 
	
	BLERemoteCharacteristic(uint16_t decl_handle,  
    uint16_t    properties,   
    uint16_t    value_handle,  
    BLEUUID     uuid,
	BLERemoteService*    pRemoteService
    );
    // Private properties
	BLEUUID              m_uuid;
	uint16_t             m_end_handle;
	uint16_t             m_handle;
	BLERemoteService*    m_pRemoteService;
	uint8_t 			 *m_rawData;
	uint16_t             m_charProp;
	std::string          m_value;
	
	
	BLERemoteService* getRemoteService();
	notify_callback	  m_notifyCallback;
    void              retrieveDescriptors();
	void              removeDescriptors();
	bool              m_haveDescriptor;
	
	std::map<std::string, BLERemoteDescriptor*> m_descriptorMap;
	BLEFreeRTOS::Semaphore m_semaphoreReadCharEvt  = BLEFreeRTOS::Semaphore("ReadCharEvt");
	BLEFreeRTOS::Semaphore  m_semaphoreRegForNotifyEvt  = BLEFreeRTOS::Semaphore("RegForNotifyEvt");
	BLEFreeRTOS::Semaphore m_semaphoregetdescEvt = BLEFreeRTOS::Semaphore("getDescriptor");
	BLEFreeRTOS::Semaphore  m_semaphoreWriteCharEvt     = BLEFreeRTOS::Semaphore("WriteCharEvt");
	
	T_APP_RESULT   clientCallbackDefault(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data);


}; // BLERemoteCharacteristic
#endif /* COMPONENTS_CPP_UTILS_BLEREMOTECHARACTERISTIC_H_ */
