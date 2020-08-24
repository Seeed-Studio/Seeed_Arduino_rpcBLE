/*
 * BLEAddress.h
 *
 *  Created on: Jul 2, 2017
 *      Author: kolban
 */

#ifndef COMPONENTS_CPP_UTILS_BLEADDRESS_H_
#define COMPONENTS_CPP_UTILS_BLEADDRESS_H_

#include <string>

/// Bluetooth address length
#define BD_ADDR_LEN     6

/// Bluetooth device address
typedef uint8_t bd_addr_t[BD_ADDR_LEN];

/**
 * @brief A %BLE device address.
 *
 * Every %BLE device has a unique address which can be used to identify it and form connections.
 */
class BLEAddress {
public:
    BLEAddress(bd_addr_t address);
	BLEAddress(std::string stringAddress);
	bool           equals(BLEAddress otherAddress);
	bd_addr_t* getNative();
	std::string    toString();
	
	uint8_t* data();

private:
	bd_addr_t m_address;    
    uint8_t _btAddr[6] = {0};   // BT address is stored MSB at _btAddr[5]    
};

#endif /* COMPONENTS_CPP_UTILS_BLEADDRESS_H_ */
