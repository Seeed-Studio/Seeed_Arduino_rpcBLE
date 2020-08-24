/*
 * BLEServer.h
 *
 *  Created on: Apr 16, 2017
 *      Author: kolban
 */

#ifndef COMPONENTS_CPP_UTILS_BLESERVER_H_
#define COMPONENTS_CPP_UTILS_BLESERVER_H_


/* TODO possibly refactor this struct */ 
typedef struct {
	void *peer_device;		// peer device BLEClient or BLEServer - maybe its better to have 2 structures or union here
	bool connected;			// do we need it?
	uint16_t mtu;			// every peer device negotiate own mtu
} conn_status_t;
#endif /* COMPONENTS_CPP_UTILS_BLESERVER_H_ */
