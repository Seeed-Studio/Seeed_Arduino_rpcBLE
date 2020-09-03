/*
 * BLEUUID.cpp
 *
 *  Created on: Jun 21, 2017
 *      Author: kolban
 */


#include <string.h>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "BLEUUID.h"
/**
 * @brief Get a string representation of the UUID.
 *
 * The format of a string is:
 * 01234567 8901 2345 6789 012345678901
 * 0000180d-0000-1000-8000-00805f9b34fb
 * 0 1 2 3  4 5  6 7  8 9  0 1 2 3 4 5
 *
 * @return A string representation of the UUID.
 */
std::string BLEUUID::toString() {
	if (!m_valueSet) return "<NULL>";   // If we have no value, nothing to format.
	// If the UUIDs are 16 or 32 bit, pad correctly.

	if (m_uuid.len == UUID_LEN_16) {  // If the UUID is 16bit, pad correctly.
		char hex[9];
		snprintf(hex, sizeof(hex), "%08x", m_uuid.uuid.uuid16);
		return std::string(hex) + "-0000-1000-8000-00805f9b34fb";
	} // End 16bit UUID

	if (m_uuid.len == UUID_LEN_32) {  // If the UUID is 32bit, pad correctly.
		char hex[9];
		snprintf(hex, sizeof(hex), "%08x", m_uuid.uuid.uuid32);
		return std::string(hex) + "-0000-1000-8000-00805f9b34fb";
	} // End 32bit UUID

	// The UUID is not 16bit or 32bit which means that it is 128bit.
	//
	// UUID string format:
	// AABBCCDD-EEFF-GGHH-IIJJ-KKLLMMNNOOPP
	auto size = 37; // 32 for UUID data, 4 for '-' delimiters and one for a terminator == 37 chars
	char *hex = (char *)malloc(size);
	snprintf(hex, size, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
			m_uuid.uuid.uuid128[15], m_uuid.uuid.uuid128[14],
			m_uuid.uuid.uuid128[13], m_uuid.uuid.uuid128[12],
			m_uuid.uuid.uuid128[11], m_uuid.uuid.uuid128[10],
			m_uuid.uuid.uuid128[9], m_uuid.uuid.uuid128[8],
			m_uuid.uuid.uuid128[7], m_uuid.uuid.uuid128[6],
			m_uuid.uuid.uuid128[5], m_uuid.uuid.uuid128[4],
			m_uuid.uuid.uuid128[3], m_uuid.uuid.uuid128[2],
			m_uuid.uuid.uuid128[1], m_uuid.uuid.uuid128[0]);
	std::string res(hex);
	free(hex);
	return res;
} // toString

BLEUUID::BLEUUID() {
}

BLEUUID::BLEUUID(const char * str) {
    char temp[] = {0, 0, 0};

    memset(_data, 0x00, sizeof(_data));

    _length = 0;
    uint8_t i;
    for (i = 0; (i <= (strlen(str) - 1)) && (_length < BLE_UUID_MAX_LENGTH); i += 2) {
        if (str[i] == '-') {
            i++;
            //continue;
        }

        temp[0] = str[i];
        temp[1] = str[i+1];

        _data[_length] = strtoul(temp, NULL, 16);

        _length++;
    }

    if (_length <= 2) {
        _length = 2;
    } else if (_length <= 4) {
        _length = 4;
    } else {
        _length = 16;
    }
    
    for (i = 0; i < _length; i++) {
        _dataNative[i] = _data[(_length - 1 - i)];
    }
}

BLEUUID::BLEUUID(uint8_t* data, uint8_t length) {
    if ((length == 2) || (length == 4) || (length == 16)) {
        _length = length;
        uint8_t i = 0;
        for (i = 0; i < _length; i++) {
            _dataNative[i] = data[i];
            _data[i] = data[(_length - 1 - i)];
        }
    }
}

const char* BLEUUID::str() {
    if (_length == 2) {
        sprintf(_str, "%2x%2x", 
                _dataNative[1], _dataNative[0]);
    }

    if (_length == 4) {
        sprintf(_str, "%2x%2x%2x%2x", 
                _dataNative[3], _dataNative[2],
                _dataNative[1], _dataNative[0]);
    }

    if (_length == 16) {
        sprintf(_str, "%2x%2x%2x%2x-%2x%2x-%2x%2x-%2x%2x-%2x%2x%2x%2x%2x%2x", 
                _dataNative[15], _dataNative[14],
                _dataNative[13], _dataNative[12],
                _dataNative[11], _dataNative[10],
                _dataNative[9], _dataNative[8],
                _dataNative[7], _dataNative[6],
                _dataNative[5], _dataNative[4],
                _dataNative[3], _dataNative[2],
                _dataNative[1], _dataNative[0]);
    }
    return _str;
}


BLEUUID::BLEUUID(uint16_t uuid) {
	m_uuid.len         = 2;
	m_uuid.uuid.uuid16 = uuid;
	m_valueSet         = true;
} // BLEUUID

/**
 * @brief Compare a UUID against this UUID.
 *
 * @param [in] uuid The UUID to compare against.
 * @return True if the UUIDs are equal and false otherwise.
 */
bool BLEUUID::equals(BLEUUID uuid) {
	//log_d("Comparing: %s to %s", toString().c_str(), uuid.toString().c_str());
	if (!m_valueSet || !uuid.m_valueSet) return false;

	if (uuid.m_uuid.len != m_uuid.len) {
		return uuid.toString() == toString();
	}

	if (uuid.m_uuid.len == UUID_LEN_16) {
		return uuid.m_uuid.uuid.uuid16 == m_uuid.uuid.uuid16;
	}

	if (uuid.m_uuid.len == UUID_LEN_32) {
		return uuid.m_uuid.uuid.uuid32 == m_uuid.uuid.uuid32;
	}

	return memcmp(uuid.m_uuid.uuid.uuid128, m_uuid.uuid.uuid128, 16) == 0;
} // equals

/**
 * @brief Get the native UUID value.
 *
 * @return The native UUID value or NULL if not set.
 */
bt_uuid_t* BLEUUID::getNative() {
	//log_d(">> getNative()")
	if (m_valueSet == false) {
		return nullptr;
	}
	return &m_uuid;
} // getNative



