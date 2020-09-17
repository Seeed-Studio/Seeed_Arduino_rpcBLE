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
	BLERemoteCharacteristic* getCharacteristic(const char* uuid);	  // Get the specified characteristic A model of a remote %BLE service..
	BLERemoteCharacteristic* getCharacteristic(BLEUUID uuid);
	std::map<std::string, BLERemoteCharacteristic*>* getCharacteristics();
	std::map<uint16_t, BLERemoteCharacteristic*>* getCharacteristicsByHandle();  // Get the characteristics map.
	void getCharacteristics(std::map<uint16_t, BLERemoteCharacteristic*>* pCharacteristicMap);
	BLEClient*               getClient(void);
	std::string              getValue(BLEUUID characteristicUuid);                      // Get the value of a characteristic.
	void                     setValue(BLEUUID characteristicUuid, std::string value);   // Set the value of a characteristic.
	uint16_t                 getHandle();
	uint16_t                 getEndHandle();
	BLEUUID                  getUUID(void);
	std::string              toString(void);
	
	
private:
   friend class BLEClient;
   BLERemoteService(uint16_t att_handle, uint16_t end_group_handle, BLEUUID uuid, BLEClient* pClient);

	
	uint16_t            getStartHandle();

	
    void            retrieveCharacteristics(void);
    void            removeCharacteristics();
	
	BLEUUID         m_uuid;
	uint16_t        m_startHandle;
	uint16_t        m_endHandle;
	BLEClient*      m_pClient;
	bool            m_haveCharacteristics;

	std::map<std::string, BLERemoteCharacteristic*> m_characteristicMap;
	std::map<uint16_t, BLERemoteCharacteristic*> m_characteristicMapByHandle;
	BLEFreeRTOS::Semaphore m_semaphoregetchaEvt = BLEFreeRTOS::Semaphore("getCharacteristic");

  	T_APP_RESULT   clientCallbackDefault(T_CLIENT_ID client_id, uint8_t conn_id, void *p_dat);
}; // BLERemoteService

#endif /* COMPONENTS_CPP_UTILS_BLEREMOTESERVICE_H_ */
