/*
 * BLEValue.cpp
 *
 *  Created on: Jul 17, 2017
 *      Author: kolban
 */


#include "BLEValue.h"



/**
 * @brief Set the current value.
 * @param [in] pData The data for the current value.
 * @param [in] The length of the new current value.
 */
void BLEValue::setValue(uint8_t* pData, size_t length) {
	m_value = std::string((char*) pData, length);
} // setValue

