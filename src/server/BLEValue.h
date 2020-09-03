/*
 * BLEValue.h
 *
 *  Created on: Jul 17, 2017
 *      Author: kolban
 */

#ifndef COMPONENTS_CPP_UTILS_BLEVALUE_H_
#define COMPONENTS_CPP_UTILS_BLEVALUE_H_
#include <string>

/**
 * @brief The model of a %BLE value.
 */
class BLEValue {
public:
    void        setValue(uint8_t* pData, size_t length);

private:
    std::string m_value;
};

#endif /* COMPONENTS_CPP_UTILS_BLEVALUE_H_ */
