#ifndef COMPONENTS_CPP_UTILS_BLEREMOTESERVICE_H_
#define COMPONENTS_CPP_UTILS_BLEREMOTESERVICE_H_

#include <map>

#include "BLEClient.h"
#include "BLEUUID.h"
#include "FreeRTOS.h"
#include "BLERemoteCharacteristic.h"
typedef uint8_t T_CLIENT_ID;

class BLEClient;
class BLERemoteCharacteristic;


/**
 * @brief A model of a remote %BLE service.
 */
class BLERemoteService {
public:	
	virtual ~BLERemoteService();
	BLERemoteCharacteristic* getCharacteristic(BLEUUID uuid);
	BLEClient*               getClient(void);
	
	static BLERemoteService*     _this;
	uint16_t                 getHandle();
	BLEUUID                  getUUID(void);
	
	
	
private:
   friend class BLEClient;
   BLERemoteService(uint16_t att_handle, uint16_t end_group_handle, uint16_t uuid16, BLEClient* pClient);
//   BLERemoteService(uint16_t att_handle,uint16_t end_group_handle,uint8_t uuid128[16],BLEClient* pClient);
	
	uint16_t            getStartHandle();

	
    void            retrieveCharacteristics(void);
    void            removeCharacteristics();
	
	uint16_t        m_srvcId16;
//	uint8_t         m_srvcId128[16];
	BLEUUID         m_uuid;
	uint16_t        m_startHandle;
	uint16_t        m_endHandle;
	BLEClient*      m_pClient;
	bool            m_haveCharacteristics;
	
	BLEFreeRTOS::Semaphore m_semaphoregetchaEvt = BLEFreeRTOS::Semaphore("getCharacteristic");

  	T_APP_RESULT   clientCallbackDefault(T_CLIENT_ID client_id, uint8_t conn_id, void *p_dat);
}; // BLERemoteService

#endif /* COMPONENTS_CPP_UTILS_BLEREMOTESERVICE_H_ */
