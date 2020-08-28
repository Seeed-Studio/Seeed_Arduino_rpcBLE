/*
 * BLEAddress.h
 *
 *  Created on: Jul 2, 2017
 *      Author: kolban
 *  Modified on: Aug 28 2020
 * 		Author: Hongtai Liu
 * 		@bref: Adapt to rtl8720D
 */
#define TAG "BLEUUID"
#ifndef COMPONENTS_CPP_UTILS_BLEADDRESS_H_
#define COMPONENTS_CPP_UTILS_BLEADDRESS_H_
#include <string>
#include "Seeed_erpcUnified.h"
/**
 * @brief A %BLE device address.
 *
 * Every %BLE device has a unique address which can be used to identify it and form connections.
 */
class BLEAddress {
public:
	BLEAddress(rtl_bd_addr_t address);
	BLEAddress(std::string stringAddress);
	bool           equals(BLEAddress otherAddress);
	rtl_bd_addr_t* getNative();
	std::string    toString();

private:
	rtl_bd_addr_t m_address;
};

#endif /* COMPONENTS_CPP_UTILS_BLEADDRESS_H_ */
