

#ifndef COMPONENTS_CPP_UTILS_BLEREMOTEDESCRIPTOR_H_
#define COMPONENTS_CPP_UTILS_BLEREMOTEDESCRIPTOR_H_


#include <string>
#include "BLERemoteCharacteristic.h"
#include "BLEUUID.h"
#include "BLEFreeRTOS.h"

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
	BLERemoteCharacteristic* m_pRemoteCharacteristic;   // 

};

#endif /* COMPONENTS_CPP_UTILS_BLEREMOTEDESCRIPTOR_H_ */
