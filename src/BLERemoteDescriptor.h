

#ifndef COMPONENTS_CPP_UTILS_BLEREMOTEDESCRIPTOR_H_
#define COMPONENTS_CPP_UTILS_BLEREMOTEDESCRIPTOR_H_


#include <string>
#include "BLERemoteCharacteristic.h"
#include "BLEUUID.h"
#include "BLEFreeRTOS.h"
#include "seeed_rpcUnified.h"
#include "rtl_ble/ble_unified.h"

class BLEClient;
class BLERemoteCharacteristic;
/**
 * @brief A model of remote %BLE descriptor.
 */
class BLERemoteDescriptor {
public:
    BLEUUID     getUUID();
	uint16_t    getHandle();
	std::string toString(void);
	BLERemoteCharacteristic* getRemoteCharacteristic();
	void        writeValue(uint8_t* data, size_t length, bool response = false);
	void        writeValue(std::string newValue, bool response = false);
	void        writeValue(uint8_t newValue, bool response = false);
    std::string readValue(void);
	uint8_t     readUInt8(void);
    uint16_t    readUInt16(void);
	uint32_t    readUInt32(void);	
private:
	friend class BLERemoteCharacteristic;
	friend class BLEClient;
	BLERemoteDescriptor(
		uint16_t                 handle,
		BLEUUID                  uuid,
		BLERemoteCharacteristic* pRemoteCharacteristic
	);
	uint16_t                 m_handle;                  // Server handle of this descriptor.
	BLEUUID                  m_uuid;                    // UUID of this descriptor.
	std::string              m_value;                   // Last received value of the descriptor.
	BLERemoteCharacteristic* m_pRemoteCharacteristic;   // 
	BLEFreeRTOS::Semaphore   m_semaphoreReadDescrEvt    = BLEFreeRTOS::Semaphore("ReadDescrEvt");

};

#endif /* COMPONENTS_CPP_UTILS_BLEREMOTEDESCRIPTOR_H_ */
