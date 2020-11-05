/*
 * BLEAdvertising.h
 *
 *  Created on: Jun 21, 2017
 *      Author: kolban
 */

#ifndef COMPONENTS_CPP_UTILS_BLEADVERTISING_H_
#define COMPONENTS_CPP_UTILS_BLEADVERTISING_H_
#include "BLEUUID.h"
#include <vector>
#include "BLEFreeRTOS.h"
#include "seeed_rpcUnified.h"
#include "rtl_ble/ble_unified.h"


/// Advertising data type
typedef enum {
    adv_data,
    adv_scan_data,
} ble_adv_data_type;



/**
 * @brief Advertisement data set by the programmer to be published by the %BLE server.
 */
class BLEAdvertisementData {
public:
	void setAppearance(uint16_t appearance);
    void setCompleteServices(BLEUUID uuid);
    void setFlags(uint8_t);
    void setManufacturerData(std::string data);
    void setName(std::string name);
    void setPartialServices(BLEUUID uuid);
    void setServiceData(BLEUUID uuid, std::string data);
    void setShortName(std::string name);
    void        addData(std::string data);  // Add data to the payload.
    std::string getPayload();               // Retrieve the current advert payload.


private:
   friend class BLEAdvertising;
   std::string m_payload;   // The payload of the advertisement.
};   // BLEAdvertisementData



/**
 * @brief Perform and manage %BLE advertising.
 *
 * A %BLE server will want to perform advertising in order to make itself known to %BLE clients.
 */
class BLEAdvertising {
public:
	BLEAdvertising();
	void start();
    void stop();
    void setAppearance(uint16_t appearance);
    void addServiceUUID(const char* serviceUUID);
	uint8_t addServiceUUID(BLEUUID serviceUUID);
	void setScanResponse(bool);
    void setMinPreferred(uint16_t);
    void setMaxPreferred(uint16_t);
    void setMaxInterval(uint16_t maxinterval);
    void setMinInterval(uint16_t mininterval);
    void setScanFilter(bool scanRequertWhitelistOnly, bool connectWhitelistOnly);
    void setDeviceAddress(uint8_t* addr, T_GAP_REMOTE_ADDR_TYPE type);   
    uint8_t addCompleteName(const char* str);
    uint8_t addShortName(const char* str);
    uint8_t addFlags(uint8_t flags);
    void setAdvData();
    void addData(const uint8_t* data, uint8_t size,ble_adv_data_type type);
    void setAdvertisementData(BLEAdvertisementData& advertisementData);
    void setScanResponseData(BLEAdvertisementData& advertisementData);
    void setAdvertisementType(uint8_t adv_type);
    T_APP_RESULT handleGAPEvent(uint8_t cb_type, void *p_cb_data);
private:
    bool                 m_customAdvData = false;  // Are we using custom advertising data?
	bool                 m_customScanResponseData = false;

    uint8_t _data[31] ={0};     // array for storing formatted advertising data for receiving and sending
    uint8_t _dataSize = 0;

    uint8_t scan_data[31] ={0};     // array for storing formatted advertising data for receiving and sending
    uint8_t scan_dataSize = 0;
    String      _devName;
    int         m_appearance;

    uint8_t     _serviceCount = 0;
    BLEUUID     _serviceList[7];
	std::vector<BLEUUID> m_serviceUUIDs;
    uint16_t _advIntMin = 320;   // Minimum advertising interval for undirected and low duty cycle directed advertising. Value range: 0x0020 - 0x4000 (20ms - 10240ms)(0.625ms/step)
    uint16_t _advIntMax = 480;  // Maximum advertising interval for undirected and low duty cycle directed advertising. Value range: 0x0020 - 0x4000 (20ms - 10240ms)(0.625ms/step)
    uint8_t  _slaveInitMtuReq = false;
    uint8_t  _advEvtType = GAP_ADTYPE_ADV_IND;
    uint8_t  _advDirectType = GAP_REMOTE_ADDR_LE_PUBLIC;
    uint8_t  _advDirectAddr[GAP_BD_ADDR_LEN] = {0};
    uint8_t  _advChannMap = GAP_ADVCHAN_ALL;
    uint8_t  _advFilterPolicy = GAP_ADV_FILTER_ANY;
  
    uint8_t _scanRspData[31] = {
                                    0x03,                             /* length */
                                    GAP_ADTYPE_APPEARANCE,            /* type="Appearance" */
                                    LO_WORD(GAP_GATT_APPEARANCE_MOUSE),
                                    HI_WORD(GAP_GATT_APPEARANCE_MOUSE),
                                    };
    uint8_t  _advDataSize = 18;
    uint8_t  _scanRspDataSize = 4;   

    uint8_t _advData[31] = {
                                    /* Flags */
                                    0x02,             /* length */
                                    GAP_ADTYPE_FLAGS, /* type="Flags" */
                                    GAP_ADTYPE_FLAGS_LIMITED | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
                                    /* Local name */
                                    0x0E,             /* length */
                                    GAP_ADTYPE_LOCAL_NAME_COMPLETE,
                                    };
	
};
#endif /* COMPONENTS_CPP_UTILS_BLEADVERTISING_H_ */
