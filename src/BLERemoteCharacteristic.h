
#ifndef COMPONENTS_CPP_UTILS_BLEREMOTECHARACTERISTIC_H_
#define COMPONENTS_CPP_UTILS_BLEREMOTECHARACTERISTIC_H_
#include <string>
#include "BLERemoteService.h"
#include "BLEUUID.h"
#include "BLEFreeRTOS.h"

class BLERemoteService;

//typedef void (*notify_callback)(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify);
/**
 * @brief A model of a remote %BLE characteristic.
 */
class BLERemoteCharacteristic {
	BLEUUID     getUUID();
    uint16_t    getHandle();
	uint16_t     getendHandle();
	std::string toString();
	
	static BLERemoteCharacteristic*     _this;
private:
    friend class BLEClient;
	friend class BLERemoteService;
	friend class BLERemoteDescriptor; 
	
	BLERemoteCharacteristic(uint16_t decl_handle,  
    uint16_t    properties,   
    uint16_t    value_handle,  
    uint16_t    uuid16,
	BLERemoteService*    pRemoteService
    );
    // Private properties
	uint16_t             m_srvcId16;
	BLEUUID              m_uuid;
	uint16_t             m_end_handle;
//    esp_gatt_auth_req_t  m_auth;
	uint16_t             m_handle;
	BLERemoteService*    m_pRemoteService;
	uint8_t 			 *m_rawData;
	
	
	BLERemoteService* getRemoteService();
//	notify_callback		 m_notifyCallback;
    void              retrieveDescriptors();
	void              removeDescriptors();
	bool              m_haveDescriptor;
	
	BLEFreeRTOS::Semaphore m_semaphoregetdescEvt = BLEFreeRTOS::Semaphore("getDescriptor");


}; // BLERemoteCharacteristic
#endif /* COMPONENTS_CPP_UTILS_BLEREMOTECHARACTERISTIC_H_ */
