/*
 * BLEAddress.h
 *
 *  Created on: Jul 2, 2017
 *      Author: kolban
 * 
 *  Modified on: Aug 28 2020
 * 		 Author: Hongtai Liu
 * 		   bref: Adapt to rtl8720D
 */
#ifndef COMPONENTS_CPP_UTILS_BLEADDRESS_H_
#define COMPONENTS_CPP_UTILS_BLEADDRESS_H_
#define TAG "BLEAddress"
#include "rpc_unified_log.h"
#include <string>
#include "Seeed_erpcUnified.h"

/**
 * @brief A %BLE device address.
 *
 * Every %BLE device has a unique address which can be used to identify it and form connections.
 */
class BLEAddress {
public:
	BLEAddress(esp_bd_addr_t address);
	BLEAddress(std::string stringAddress);
	bool           equals(BLEAddress otherAddress);
	esp_bd_addr_t* getNative();
	std::string    toString();

private:
	esp_bd_addr_t m_address;
};


#endif /* COMPONENTS_CPP_UTILS_BLEADDRESS_H_ */
