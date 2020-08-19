/*
 * BLEUUID.h
 *
 *  Created on: Jun 21, 2017
 *      Author: kolban
 */

#ifndef COMPONENTS_CPP_UTILS_BLEUUID_H_
#define COMPONENTS_CPP_UTILS_BLEUUID_H_

#include <string>

/// UUID type
typedef struct {
#define UUID_LEN_16     2
#define UUID_LEN_32     4
#define UUID_LEN_128    16
    uint16_t len;							/*!< UUID length, 16bit, 32bit or 128bit */
    union {
        uint16_t    uuid16;
        uint32_t    uuid32;
        uint8_t     uuid128[UUID_LEN_128];
    } uuid;									/*!< UUID */
} __attribute__((packed)) bt_uuid_t;


/**
 * @brief A model of a %BLE UUID.
 */
class BLEUUID {
public:
	std::string    toString();

private:
    bool          m_valueSet = false;   // Is there a value set for this instance.
	bt_uuid_t m_uuid;       		// The underlying UUID structure that this class wraps.
}; // BLEUUID
#endif /* COMPONENTS_CPP_UTILS_BLEUUID_H_ */
