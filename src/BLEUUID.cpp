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

