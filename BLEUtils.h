/*
 * BLEUtils.h
 *
 *  Created on: Mar 25, 2017
 *      Author: kolban
 *	Modified on: Aug 28, 2020
 *       Author: Hingtai Liu
 *  bref:
 */

#ifndef COMPONENTS_CPP_UTILS_BLEUTILS_H_
#define COMPONENTS_CPP_UTILS_BLEUTILS_H_
#include <string>
#include "Seeed_erpcUnified.h"

/**
 * @brief A set of general %BLE utilities.
 */
class BLEUtils {
public:
	static const char*        addressTypeToString(esp_ble_addr_type_t type);
	static std::string        adFlagsToString(uint8_t adFlags);
	static const char*        advTypeToString(uint8_t advType);
	static char*              buildHexData(uint8_t* target, uint8_t* source, uint8_t length);
	static std::string        buildPrintData(uint8_t* source, size_t length);
};

#endif /* COMPONENTS_CPP_UTILS_BLEUTILS_H_ */
