/*
 * BLEUUID.h
 *
 *  Created on: Jun 21, 2017
 *      Author: kolban
 */

#ifndef COMPONENTS_CPP_UTILS_BLEUUID_H_
#define COMPONENTS_CPP_UTILS_BLEUUID_H_

#include <string>
#include <Arduino.h>
#define BLE_UUID_MAX_LENGTH 16

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
    BLEUUID();
    BLEUUID(const char * str);      // Build a BLEUUID object from an UUID string. Use of hyphens (-) to seperate UUID sections in the string is acceptable.
    BLEUUID(uint8_t* data, uint8_t length);	
	const char* str();
	bool           equals(BLEUUID uuid);
	bt_uuid_t* getNative();

private:
    bool          m_valueSet = false;   // Is there a value set for this instance.
	bt_uuid_t m_uuid;       		// The underlying UUID structure that this class wraps.
	
    char _str[BLE_UUID_MAX_LENGTH * 2 + 5] = {0};
    uint8_t _data[BLE_UUID_MAX_LENGTH] = {0};           // stores the UUID MSB in position 0, useful for printing as characters/strings
    uint8_t _dataNative[BLE_UUID_MAX_LENGTH] = {0};     // stores the UUID LSB in position 0
    uint8_t _length = 0;    // number of bytes(characters) of UUID	
}; // BLEUUID
#endif /* COMPONENTS_CPP_UTILS_BLEUUID_H_ */
