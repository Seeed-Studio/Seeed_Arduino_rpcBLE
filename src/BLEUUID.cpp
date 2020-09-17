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


/**
 * @brief Convert a UUID to its 128 bit representation.
 *
 * A UUID can be internally represented as 16bit, 32bit or the full 128bit.  This method
 * will convert 16 or 32 bit representations to the full 128bit.
 */
BLEUUID BLEUUID::to128() {
	//log_v(">> toFull() - %s", toString().c_str());

	// If we either don't have a value or are already a 128 bit UUID, nothing further to do.
	if (!m_valueSet || m_uuid.len == UUID_LEN_128) {
		return *this;
	}

	// If we are 16 bit or 32 bit, then set the 4 bytes of the variable part of the UUID.
	if (m_uuid.len == UUID_LEN_16) {
		uint16_t temp = m_uuid.uuid.uuid16;
		m_uuid.uuid.uuid128[15] = 0;
		m_uuid.uuid.uuid128[14] = 0;
		m_uuid.uuid.uuid128[13] = (temp >> 8) & 0xff;
		m_uuid.uuid.uuid128[12] = temp & 0xff;

	}
	else if (m_uuid.len == UUID_LEN_32) {
		uint32_t temp = m_uuid.uuid.uuid32;
		m_uuid.uuid.uuid128[15] = (temp >> 24) & 0xff;
		m_uuid.uuid.uuid128[14] = (temp >> 16) & 0xff;
		m_uuid.uuid.uuid128[13] = (temp >> 8) & 0xff;
		m_uuid.uuid.uuid128[12] = temp & 0xff;
	}

	// Set the fixed parts of the UUID.
	m_uuid.uuid.uuid128[11] = 0x00;
	m_uuid.uuid.uuid128[10] = 0x00;

	m_uuid.uuid.uuid128[9]  = 0x10;
	m_uuid.uuid.uuid128[8]  = 0x00;

	m_uuid.uuid.uuid128[7]  = 0x80;
	m_uuid.uuid.uuid128[6]  = 0x00;

	m_uuid.uuid.uuid128[5]  = 0x00;
	m_uuid.uuid.uuid128[4]  = 0x80;
	m_uuid.uuid.uuid128[3]  = 0x5f;
	m_uuid.uuid.uuid128[2]  = 0x9b;
	m_uuid.uuid.uuid128[1]  = 0x34;
	m_uuid.uuid.uuid128[0]  = 0xfb;

	m_uuid.len = UUID_LEN_128;

	return *this;
} // to128


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


uint8_t BLEUUID::length() {
    return _length;
}

const uint8_t* BLEUUID::dataNative() {
    return _dataNative;
}

static void memrcpy(uint8_t* target, uint8_t* source, uint32_t size) {
	assert(size > 0);
	target += (size - 1); // Point target to the last byte of the target data
	while (size > 0) {
		*target = *source;
		target--;
		source++;
		size--;
	}
} // memrcpy


/**
 * @brief Create a UUID from a string.
 *
 * Create a UUID from a string.  There will be two possible stories here.  Either the string represents
 * a binary data field or the string represents a hex encoding of a UUID.
 * For the hex encoding, here is an example:
 *
 * ```
 * "beb5483e-36e1-4688-b7f5-ea07361b26a8"
 *  0 1 2 3  4 5  6 7  8 9  0 1 2 3 4 5
 *  12345678-90ab-cdef-1234-567890abcdef
 * ```
 *
 * This has a length of 36 characters.  We need to parse this into 16 bytes.
 *
 * @param [in] value The string to build a UUID from.
 */
BLEUUID::BLEUUID(std::string value) {
	m_valueSet = true;
	if (value.length() == 4) {
		m_uuid.len         = UUID_LEN_16;
		m_uuid.uuid.uuid16 = 0;
		for(int i=0;i<value.length();){
			uint8_t MSB = value.c_str()[i];
			uint8_t LSB = value.c_str()[i+1];
			
			if(MSB > '9') MSB -= 7;
			if(LSB > '9') LSB -= 7;
			m_uuid.uuid.uuid16 += (((MSB&0x0F) <<4) | (LSB & 0x0F))<<(2-i)*4;
			i+=2;	
		}
	}
	else if (value.length() == 8) {
		m_uuid.len         = UUID_LEN_32;
		m_uuid.uuid.uuid32 = 0;
		for(int i=0;i<value.length();){
			uint8_t MSB = value.c_str()[i];
			uint8_t LSB = value.c_str()[i+1];
			
			if(MSB > '9') MSB -= 7; 
			if(LSB > '9') LSB -= 7;
			m_uuid.uuid.uuid32 += (((MSB&0x0F) <<4) | (LSB & 0x0F))<<(6-i)*4;
			i+=2;
		}		
	}
	else if (value.length() == 16) {  // how we can have 16 byte length string reprezenting 128 bit uuid??? needs to be investigated (lack of time)
		m_uuid.len = UUID_LEN_128;
		memrcpy(m_uuid.uuid.uuid128, (uint8_t*)value.data(), 16);
	}
	else if (value.length() == 36) {
		// If the length of the string is 36 bytes then we will assume it is a long hex string in
		// UUID format.
		m_uuid.len = UUID_LEN_128;
		int n = 0;
		for(int i=0;i<value.length();){
			if(value.c_str()[i] == '-')
				i++;
			uint8_t MSB = value.c_str()[i];
			uint8_t LSB = value.c_str()[i+1];
			
			if(MSB > '9') MSB -= 7; 
			if(LSB > '9') LSB -= 7;
			m_uuid.uuid.uuid128[15-n++] = ((MSB&0x0F) <<4) | (LSB & 0x0F);
			i+=2;	
		}
	}
	else {
		m_valueSet = false;
	}
} //BLEUUID(std::string)


/**
 * @brief Create a UUID from 16 bytes of memory.
 *
 * @param [in] pData The pointer to the start of the UUID.
 * @param [in] size The size of the data.
 * @param [in] msbFirst Is the MSB first in pData memory?
 */
BLEUUID::BLEUUID(uint8_t* pData, size_t size, bool msbFirst) {
	if (size != 16) {
		return;
	}
	m_uuid.len = UUID_LEN_128;
	if (msbFirst) {
		memrcpy(m_uuid.uuid.uuid128, pData, 16);
	} else {
		memcpy(m_uuid.uuid.uuid128, pData, 16);
	}
	m_valueSet = true;
} // BLEUUID


/**
 * @brief Get the number of bits in this uuid.
 * @return The number of bits in the UUID.  One of 16, 32 or 128.
 */
uint8_t BLEUUID::bitSize() {
	if (!m_valueSet) return 0;
	switch (m_uuid.len) {
		case UUID_LEN_16:
			return 16;
		case UUID_LEN_32:
			return 32;
		case UUID_LEN_128:
			return 128;
		default:
			return 0;
	} // End of switch
} // bitSize
