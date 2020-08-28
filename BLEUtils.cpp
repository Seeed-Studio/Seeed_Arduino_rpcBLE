/*
 * BLEUtils.cpp
 *
 *  Created on: Mar 25, 2017
 *      Author: kolban
 */
#define TAG "BLEUUID"
#include "BLELog.h"

#include "BLEAddress.h"
#include "BLEUtils.h"
#include "BLEUUID.h"
#include <map> // MORE of C++ STL
#include <sstream>
#include <iomanip>

/*
static std::map<std::string, BLEClient*> g_addressMap;
static std::map<uint16_t, BLEClient*> g_connIdMap;
*/

typedef struct
{
	uint32_t assignedNumber;
	const char *name;
} member_t;

static const member_t members_ids[] = {
#ifdef ENABLE_RPC_VERBOSE
	{0xFE08, "Microsoft"},
	{0xFE09, "Pillsy, Inc."},
	{0xFE0A, "ruwido austria gmbh"},
	{0xFE0B, "ruwido austria gmbh"},
	{0xFE0C, "Procter & Gamble"},
	{0xFE0D, "Procter & Gamble"},
	{0xFE0E, "Setec Pty Ltd"},
	{0xFE0F, "Philips Lighting B.V."},
	{0xFE10, "Lapis Semiconductor Co., Ltd."},
	{0xFE11, "GMC-I Messtechnik GmbH"},
	{0xFE12, "M-Way Solutions GmbH"},
	{0xFE13, "Apple Inc."},
	{0xFE14, "Flextronics International USA Inc."},
	{0xFE15, "Amazon Fulfillment Services, Inc."},
	{0xFE16, "Footmarks, Inc."},
	{0xFE17, "Telit Wireless Solutions GmbH"},
	{0xFE18, "Runtime, Inc."},
	{0xFE19, "Google Inc."},
	{0xFE1A, "Tyto Life LLC"},
	{0xFE1B, "Tyto Life LLC"},
	{0xFE1C, "NetMedia, Inc."},
	{0xFE1D, "Illuminati Instrument Corporation"},
	{0xFE1E, "Smart Innovations Co., Ltd"},
	{0xFE1F, "Garmin International, Inc."},
	{0xFE20, "Emerson"},
	{0xFE21, "Bose Corporation"},
	{0xFE22, "Zoll Medical Corporation"},
	{0xFE23, "Zoll Medical Corporation"},
	{0xFE24, "August Home Inc"},
	{0xFE25, "Apple, Inc. "},
	{0xFE26, "Google Inc."},
	{0xFE27, "Google Inc."},
	{0xFE28, "Ayla Networks"},
	{0xFE29, "Gibson Innovations"},
	{0xFE2A, "DaisyWorks, Inc."},
	{0xFE2B, "ITT Industries"},
	{0xFE2C, "Google Inc."},
	{0xFE2D, "SMART INNOVATION Co.,Ltd"},
	{0xFE2E, "ERi,Inc."},
	{0xFE2F, "CRESCO Wireless, Inc"},
	{0xFE30, "Volkswagen AG"},
	{0xFE31, "Volkswagen AG"},
	{0xFE32, "Pro-Mark, Inc."},
	{0xFE33, "CHIPOLO d.o.o."},
	{0xFE34, "SmallLoop LLC"},
	{0xFE35, "HUAWEI Technologies Co., Ltd"},
	{0xFE36, "HUAWEI Technologies Co., Ltd"},
	{0xFE37, "Spaceek LTD"},
	{0xFE38, "Spaceek LTD"},
	{0xFE39, "TTS Tooltechnic Systems AG & Co. KG"},
	{0xFE3A, "TTS Tooltechnic Systems AG & Co. KG"},
	{0xFE3B, "Dolby Laboratories"},
	{0xFE3C, "Alibaba"},
	{0xFE3D, "BD Medical"},
	{0xFE3E, "BD Medical"},
	{0xFE3F, "Friday Labs Limited"},
	{0xFE40, "Inugo Systems Limited"},
	{0xFE41, "Inugo Systems Limited"},
	{0xFE42, "Nets A/S "},
	{0xFE43, "Andreas Stihl AG & Co. KG"},
	{0xFE44, "SK Telecom "},
	{0xFE45, "Snapchat Inc"},
	{0xFE46, "B&O Play A/S "},
	{0xFE47, "General Motors"},
	{0xFE48, "General Motors"},
	{0xFE49, "SenionLab AB"},
	{0xFE4A, "OMRON HEALTHCARE Co., Ltd."},
	{0xFE4B, "Philips Lighting B.V."},
	{0xFE4C, "Volkswagen AG"},
	{0xFE4D, "Casambi Technologies Oy"},
	{0xFE4E, "NTT docomo"},
	{0xFE4F, "Molekule, Inc."},
	{0xFE50, "Google Inc."},
	{0xFE51, "SRAM"},
	{0xFE52, "SetPoint Medical"},
	{0xFE53, "3M"},
	{0xFE54, "Motiv, Inc."},
	{0xFE55, "Google Inc."},
	{0xFE56, "Google Inc."},
	{0xFE57, "Dotted Labs"},
	{0xFE58, "Nordic Semiconductor ASA"},
	{0xFE59, "Nordic Semiconductor ASA"},
	{0xFE5A, "Chronologics Corporation"},
	{0xFE5B, "GT-tronics HK Ltd"},
	{0xFE5C, "million hunters GmbH"},
	{0xFE5D, "Grundfos A/S"},
	{0xFE5E, "Plastc Corporation"},
	{0xFE5F, "Eyefi, Inc."},
	{0xFE60, "Lierda Science & Technology Group Co., Ltd."},
	{0xFE61, "Logitech International SA"},
	{0xFE62, "Indagem Tech LLC"},
	{0xFE63, "Connected Yard, Inc."},
	{0xFE64, "Siemens AG"},
	{0xFE65, "CHIPOLO d.o.o."},
	{0xFE66, "Intel Corporation"},
	{0xFE67, "Lab Sensor Solutions"},
	{0xFE68, "Qualcomm Life Inc"},
	{0xFE69, "Qualcomm Life Inc"},
	{0xFE6A, "Kontakt Micro-Location Sp. z o.o."},
	{0xFE6B, "TASER International, Inc."},
	{0xFE6C, "TASER International, Inc."},
	{0xFE6D, "The University of Tokyo"},
	{0xFE6E, "The University of Tokyo"},
	{0xFE6F, "LINE Corporation"},
	{0xFE70, "Beijing Jingdong Century Trading Co., Ltd."},
	{0xFE71, "Plume Design Inc"},
	{0xFE72, "St. Jude Medical, Inc."},
	{0xFE73, "St. Jude Medical, Inc."},
	{0xFE74, "unwire"},
	{0xFE75, "TangoMe"},
	{0xFE76, "TangoMe"},
	{0xFE77, "Hewlett-Packard Company"},
	{0xFE78, "Hewlett-Packard Company"},
	{0xFE79, "Zebra Technologies"},
	{0xFE7A, "Bragi GmbH"},
	{0xFE7B, "Orion Labs, Inc."},
	{0xFE7C, "Telit Wireless Solutions (Formerly Stollmann E+V GmbH)"},
	{0xFE7D, "Aterica Health Inc."},
	{0xFE7E, "Awear Solutions Ltd"},
	{0xFE7F, "Doppler Lab"},
	{0xFE80, "Doppler Lab"},
	{0xFE81, "Medtronic Inc."},
	{0xFE82, "Medtronic Inc."},
	{0xFE83, "Blue Bite"},
	{0xFE84, "RF Digital Corp"},
	{0xFE85, "RF Digital Corp"},
	{0xFE86, "HUAWEI Technologies Co., Ltd. ( )"},
	{0xFE87, "Qingdao Yeelink Information Technology Co., Ltd. ( )"},
	{0xFE88, "SALTO SYSTEMS S.L."},
	{0xFE89, "B&O Play A/S"},
	{0xFE8A, "Apple, Inc."},
	{0xFE8B, "Apple, Inc."},
	{0xFE8C, "TRON Forum"},
	{0xFE8D, "Interaxon Inc."},
	{0xFE8E, "ARM Ltd"},
	{0xFE8F, "CSR"},
	{0xFE90, "JUMA"},
	{0xFE91, "Shanghai Imilab Technology Co.,Ltd"},
	{0xFE92, "Jarden Safety & Security"},
	{0xFE93, "OttoQ Inc."},
	{0xFE94, "OttoQ Inc."},
	{0xFE95, "Xiaomi Inc."},
	{0xFE96, "Tesla Motor Inc."},
	{0xFE97, "Tesla Motor Inc."},
	{0xFE98, "Currant, Inc."},
	{0xFE99, "Currant, Inc."},
	{0xFE9A, "Estimote"},
	{0xFE9B, "Samsara Networks, Inc"},
	{0xFE9C, "GSI Laboratories, Inc."},
	{0xFE9D, "Mobiquity Networks Inc"},
	{0xFE9E, "Dialog Semiconductor B.V."},
	{0xFE9F, "Google Inc."},
	{0xFEA0, "Google Inc."},
	{0xFEA1, "Intrepid Control Systems, Inc."},
	{0xFEA2, "Intrepid Control Systems, Inc."},
	{0xFEA3, "ITT Industries"},
	{0xFEA4, "Paxton Access Ltd"},
	{0xFEA5, "GoPro, Inc."},
	{0xFEA6, "GoPro, Inc."},
	{0xFEA7, "UTC Fire and Security"},
	{0xFEA8, "Savant Systems LLC"},
	{0xFEA9, "Savant Systems LLC"},
	{0xFEAA, "Google Inc."},
	{0xFEAB, "Nokia Corporation"},
	{0xFEAC, "Nokia Corporation"},
	{0xFEAD, "Nokia Corporation"},
	{0xFEAE, "Nokia Corporation"},
	{0xFEAF, "Nest Labs Inc."},
	{0xFEB0, "Nest Labs Inc."},
	{0xFEB1, "Electronics Tomorrow Limited"},
	{0xFEB2, "Microsoft Corporation"},
	{0xFEB3, "Taobao"},
	{0xFEB4, "WiSilica Inc."},
	{0xFEB5, "WiSilica Inc."},
	{0xFEB6, "Vencer Co, Ltd"},
	{0xFEB7, "Facebook, Inc."},
	{0xFEB8, "Facebook, Inc."},
	{0xFEB9, "LG Electronics"},
	{0xFEBA, "Tencent Holdings Limited"},
	{0xFEBB, "adafruit industries"},
	{0xFEBC, "Dexcom, Inc. "},
	{0xFEBD, "Clover Network, Inc."},
	{0xFEBE, "Bose Corporation"},
	{0xFEBF, "Nod, Inc."},
	{0xFEC0, "KDDI Corporation"},
	{0xFEC1, "KDDI Corporation"},
	{0xFEC2, "Blue Spark Technologies, Inc."},
	{0xFEC3, "360fly, Inc."},
	{0xFEC4, "PLUS Location Systems"},
	{0xFEC5, "Realtek Semiconductor Corp."},
	{0xFEC6, "Kocomojo, LLC"},
	{0xFEC7, "Apple, Inc."},
	{0xFEC8, "Apple, Inc."},
	{0xFEC9, "Apple, Inc."},
	{0xFECA, "Apple, Inc."},
	{0xFECB, "Apple, Inc."},
	{0xFECC, "Apple, Inc."},
	{0xFECD, "Apple, Inc."},
	{0xFECE, "Apple, Inc."},
	{0xFECF, "Apple, Inc."},
	{0xFED0, "Apple, Inc."},
	{0xFED1, "Apple, Inc."},
	{0xFED2, "Apple, Inc."},
	{0xFED3, "Apple, Inc."},
	{0xFED4, "Apple, Inc."},
	{0xFED5, "Plantronics Inc."},
	{0xFED6, "Broadcom Corporation"},
	{0xFED7, "Broadcom Corporation"},
	{0xFED8, "Google Inc."},
	{0xFED9, "Pebble Technology Corporation"},
	{0xFEDA, "ISSC Technologies Corporation"},
	{0xFEDB, "Perka, Inc."},
	{0xFEDC, "Jawbone"},
	{0xFEDD, "Jawbone"},
	{0xFEDE, "Coin, Inc."},
	{0xFEDF, "Design SHIFT"},
	{0xFEE0, "Anhui Huami Information Technology Co."},
	{0xFEE1, "Anhui Huami Information Technology Co."},
	{0xFEE2, "Anki, Inc."},
	{0xFEE3, "Anki, Inc."},
	{0xFEE4, "Nordic Semiconductor ASA"},
	{0xFEE5, "Nordic Semiconductor ASA"},
	{0xFEE6, "Silvair, Inc."},
	{0xFEE7, "Tencent Holdings Limited"},
	{0xFEE8, "Quintic Corp."},
	{0xFEE9, "Quintic Corp."},
	{0xFEEA, "Swirl Networks, Inc."},
	{0xFEEB, "Swirl Networks, Inc."},
	{0xFEEC, "Tile, Inc."},
	{0xFEED, "Tile, Inc."},
	{0xFEEE, "Polar Electro Oy"},
	{0xFEEF, "Polar Electro Oy"},
	{0xFEF0, "Intel"},
	{0xFEF1, "CSR"},
	{0xFEF2, "CSR"},
	{0xFEF3, "Google Inc."},
	{0xFEF4, "Google Inc."},
	{0xFEF5, "Dialog Semiconductor GmbH"},
	{0xFEF6, "Wicentric, Inc."},
	{0xFEF7, "Aplix Corporation"},
	{0xFEF8, "Aplix Corporation"},
	{0xFEF9, "PayPal, Inc."},
	{0xFEFA, "PayPal, Inc."},
	{0xFEFB, "Telit Wireless Solutions (Formerly Stollmann E+V GmbH)"},
	{0xFEFC, "Gimbal, Inc."},
	{0xFEFD, "Gimbal, Inc."},
	{0xFEFE, "GN ReSound A/S"},
	{0xFEFF, "GN Netcom"},
	{0xFFFF, "Reserved"}, /*for testing purposes only*/
#endif
	{0, ""}};

typedef struct
{
	uint32_t assignedNumber;
	const char *name;
} gattdescriptor_t;

static const gattdescriptor_t g_descriptor_ids[] = {
#ifdef ENABLE_RPC_VERBOSE
	{0x2905, "Characteristic Aggregate Format"},
	{0x2900, "Characteristic Extended Properties"},
	{0x2904, "Characteristic Presentation Format"},
	{0x2901, "Characteristic User Description"},
	{0x2902, "Client Characteristic Configuration"},
	{0x290B, "Environmental Sensing Configuration"},
	{0x290C, "Environmental Sensing Measurement"},
	{0x290D, "Environmental Sensing Trigger Setting"},
	{0x2907, "External Report Reference"},
	{0x2909, "Number of Digitals"},
	{0x2908, "Report Reference"},
	{0x2903, "Server Characteristic Configuration"},
	{0x290E, "Time Trigger Setting"},
	{0x2906, "Valid Range"},
	{0x290A, "Value Trigger Setting"},
#endif
	{0, ""}};

typedef struct
{
	uint32_t assignedNumber;
	const char *name;
} characteristicMap_t;

static const characteristicMap_t g_characteristicsMappings[] = {
#ifdef ENABLE_RPC_VERBOSE
	{0x2A7E, "Aerobic Heart Rate Lower Limit"},
	{0x2A84, "Aerobic Heart Rate Upper Limit"},
	{0x2A7F, "Aerobic Threshold"},
	{0x2A80, "Age"},
	{0x2A5A, "Aggregate"},
	{0x2A43, "Alert Category ID"},
	{0x2A42, "Alert Category ID Bit Mask"},
	{0x2A06, "Alert Level"},
	{0x2A44, "Alert Notification Control Point"},
	{0x2A3F, "Alert Status"},
	{0x2AB3, "Altitude"},
	{0x2A81, "Anaerobic Heart Rate Lower Limit"},
	{0x2A82, "Anaerobic Heart Rate Upper Limit"},
	{0x2A83, "Anaerobic Threshold"},
	{0x2A58, "Analog"},
	{0x2A59, "Analog Output"},
	{0x2A73, "Apparent Wind Direction"},
	{0x2A72, "Apparent Wind Speed"},
	{0x2A01, "Appearance"},
	{0x2AA3, "Barometric Pressure Trend"},
	{0x2A19, "Battery Level"},
	{0x2A1B, "Battery Level State"},
	{0x2A1A, "Battery Power State"},
	{0x2A49, "Blood Pressure Feature"},
	{0x2A35, "Blood Pressure Measurement"},
	{0x2A9B, "Body Composition Feature"},
	{0x2A9C, "Body Composition Measurement"},
	{0x2A38, "Body Sensor Location"},
	{0x2AA4, "Bond Management Control Point"},
	{0x2AA5, "Bond Management Features"},
	{0x2A22, "Boot Keyboard Input Report"},
	{0x2A32, "Boot Keyboard Output Report"},
	{0x2A33, "Boot Mouse Input Report"},
	{0x2AA6, "Central Address Resolution"},
	{0x2AA8, "CGM Feature"},
	{0x2AA7, "CGM Measurement"},
	{0x2AAB, "CGM Session Run Time"},
	{0x2AAA, "CGM Session Start Time"},
	{0x2AAC, "CGM Specific Ops Control Point"},
	{0x2AA9, "CGM Status"},
	{0x2ACE, "Cross Trainer Data"},
	{0x2A5C, "CSC Feature"},
	{0x2A5B, "CSC Measurement"},
	{0x2A2B, "Current Time"},
	{0x2A66, "Cycling Power Control Point"},
	{0x2A66, "Cycling Power Control Point"},
	{0x2A65, "Cycling Power Feature"},
	{0x2A65, "Cycling Power Feature"},
	{0x2A63, "Cycling Power Measurement"},
	{0x2A64, "Cycling Power Vector"},
	{0x2A99, "Database Change Increment"},
	{0x2A85, "Date of Birth"},
	{0x2A86, "Date of Threshold Assessment"},
	{0x2A08, "Date Time"},
	{0x2A0A, "Day Date Time"},
	{0x2A09, "Day of Week"},
	{0x2A7D, "Descriptor Value Changed"},
	{0x2A00, "Device Name"},
	{0x2A7B, "Dew Point"},
	{0x2A56, "Digital"},
	{0x2A57, "Digital Output"},
	{0x2A0D, "DST Offset"},
	{0x2A6C, "Elevation"},
	{0x2A87, "Email Address"},
	{0x2A0B, "Exact Time 100"},
	{0x2A0C, "Exact Time 256"},
	{0x2A88, "Fat Burn Heart Rate Lower Limit"},
	{0x2A89, "Fat Burn Heart Rate Upper Limit"},
	{0x2A26, "Firmware Revision String"},
	{0x2A8A, "First Name"},
	{0x2AD9, "Fitness Machine Control Point"},
	{0x2ACC, "Fitness Machine Feature"},
	{0x2ADA, "Fitness Machine Status"},
	{0x2A8B, "Five Zone Heart Rate Limits"},
	{0x2AB2, "Floor Number"},
	{0x2A8C, "Gender"},
	{0x2A51, "Glucose Feature"},
	{0x2A18, "Glucose Measurement"},
	{0x2A34, "Glucose Measurement Context"},
	{0x2A74, "Gust Factor"},
	{0x2A27, "Hardware Revision String"},
	{0x2A39, "Heart Rate Control Point"},
	{0x2A8D, "Heart Rate Max"},
	{0x2A37, "Heart Rate Measurement"},
	{0x2A7A, "Heat Index"},
	{0x2A8E, "Height"},
	{0x2A4C, "HID Control Point"},
	{0x2A4A, "HID Information"},
	{0x2A8F, "Hip Circumference"},
	{0x2ABA, "HTTP Control Point"},
	{0x2AB9, "HTTP Entity Body"},
	{0x2AB7, "HTTP Headers"},
	{0x2AB8, "HTTP Status Code"},
	{0x2ABB, "HTTPS Security"},
	{0x2A6F, "Humidity"},
	{0x2A2A, "IEEE 11073-20601 Regulatory Certification Data List"},
	{0x2AD2, "Indoor Bike Data"},
	{0x2AAD, "Indoor Positioning Configuration"},
	{0x2A36, "Intermediate Cuff Pressure"},
	{0x2A1E, "Intermediate Temperature"},
	{0x2A77, "Irradiance"},
	{0x2AA2, "Language"},
	{0x2A90, "Last Name"},
	{0x2AAE, "Latitude"},
	{0x2A6B, "LN Control Point"},
	{0x2A6A, "LN Feature"},
	{0x2AB1, "Local East Coordinate"},
	{0x2AB0, "Local North Coordinate"},
	{0x2A0F, "Local Time Information"},
	{0x2A67, "Location and Speed Characteristic"},
	{0x2AB5, "Location Name"},
	{0x2AAF, "Longitude"},
	{0x2A2C, "Magnetic Declination"},
	{0x2AA0, "Magnetic Flux Density - 2D"},
	{0x2AA1, "Magnetic Flux Density - 3D"},
	{0x2A29, "Manufacturer Name String"},
	{0x2A91, "Maximum Recommended Heart Rate"},
	{0x2A21, "Measurement Interval"},
	{0x2A24, "Model Number String"},
	{0x2A68, "Navigation"},
	{0x2A3E, "Network Availability"},
	{0x2A46, "New Alert"},
	{0x2AC5, "Object Action Control Point"},
	{0x2AC8, "Object Changed"},
	{0x2AC1, "Object First-Created"},
	{0x2AC3, "Object ID"},
	{0x2AC2, "Object Last-Modified"},
	{0x2AC6, "Object List Control Point"},
	{0x2AC7, "Object List Filter"},
	{0x2ABE, "Object Name"},
	{0x2AC4, "Object Properties"},
	{0x2AC0, "Object Size"},
	{0x2ABF, "Object Type"},
	{0x2ABD, "OTS Feature"},
	{0x2A04, "Peripheral Preferred Connection Parameters"},
	{0x2A02, "Peripheral Privacy Flag"},
	{0x2A5F, "PLX Continuous Measurement Characteristic"},
	{0x2A60, "PLX Features"},
	{0x2A5E, "PLX Spot-Check Measurement"},
	{0x2A50, "PnP ID"},
	{0x2A75, "Pollen Concentration"},
	{0x2A2F, "Position 2D"},
	{0x2A30, "Position 3D"},
	{0x2A69, "Position Quality"},
	{0x2A6D, "Pressure"},
	{0x2A4E, "Protocol Mode"},
	{0x2A62, "Pulse Oximetry Control Point"},
	{0x2A60, "Pulse Oximetry Pulsatile Event Characteristic"},
	{0x2A78, "Rainfall"},
	{0x2A03, "Reconnection Address"},
	{0x2A52, "Record Access Control Point"},
	{0x2A14, "Reference Time Information"},
	{0x2A3A, "Removable"},
	{0x2A4D, "Report"},
	{0x2A4B, "Report Map"},
	{0x2AC9, "Resolvable Private Address Only"},
	{0x2A92, "Resting Heart Rate"},
	{0x2A40, "Ringer Control point"},
	{0x2A41, "Ringer Setting"},
	{0x2AD1, "Rower Data"},
	{0x2A54, "RSC Feature"},
	{0x2A53, "RSC Measurement"},
	{0x2A55, "SC Control Point"},
	{0x2A4F, "Scan Interval Window"},
	{0x2A31, "Scan Refresh"},
	{0x2A3C, "Scientific Temperature Celsius"},
	{0x2A10, "Secondary Time Zone"},
	{0x2A5D, "Sensor Location"},
	{0x2A25, "Serial Number String"},
	{0x2A05, "Service Changed"},
	{0x2A3B, "Service Required"},
	{0x2A28, "Software Revision String"},
	{0x2A93, "Sport Type for Aerobic and Anaerobic Thresholds"},
	{0x2AD0, "Stair Climber Data"},
	{0x2ACF, "Step Climber Data"},
	{0x2A3D, "String"},
	{0x2AD7, "Supported Heart Rate Range"},
	{0x2AD5, "Supported Inclination Range"},
	{0x2A47, "Supported New Alert Category"},
	{0x2AD8, "Supported Power Range"},
	{0x2AD6, "Supported Resistance Level Range"},
	{0x2AD4, "Supported Speed Range"},
	{0x2A48, "Supported Unread Alert Category"},
	{0x2A23, "System ID"},
	{0x2ABC, "TDS Control Point"},
	{0x2A6E, "Temperature"},
	{0x2A1F, "Temperature Celsius"},
	{0x2A20, "Temperature Fahrenheit"},
	{0x2A1C, "Temperature Measurement"},
	{0x2A1D, "Temperature Type"},
	{0x2A94, "Three Zone Heart Rate Limits"},
	{0x2A12, "Time Accuracy"},
	{0x2A15, "Time Broadcast"},
	{0x2A13, "Time Source"},
	{0x2A16, "Time Update Control Point"},
	{0x2A17, "Time Update State"},
	{0x2A11, "Time with DST"},
	{0x2A0E, "Time Zone"},
	{0x2AD3, "Training Status"},
	{0x2ACD, "Treadmill Data"},
	{0x2A71, "True Wind Direction"},
	{0x2A70, "True Wind Speed"},
	{0x2A95, "Two Zone Heart Rate Limit"},
	{0x2A07, "Tx Power Level"},
	{0x2AB4, "Uncertainty"},
	{0x2A45, "Unread Alert Status"},
	{0x2AB6, "URI"},
	{0x2A9F, "User Control Point"},
	{0x2A9A, "User Index"},
	{0x2A76, "UV Index"},
	{0x2A96, "VO2 Max"},
	{0x2A97, "Waist Circumference"},
	{0x2A98, "Weight"},
	{0x2A9D, "Weight Measurement"},
	{0x2A9E, "Weight Scale Feature"},
	{0x2A79, "Wind Chill"},
#endif
	{0, ""}};

/**
 * @brief Mapping from service ids to names
 */
typedef struct
{
	const char *name;
	const char *type;
	uint32_t assignedNumber;
} gattService_t;

/**
 * Definition of the service ids to names that we know about.
 */
static const gattService_t g_gattServices[] = {
#ifdef ENABLE_RPC_VERBOSE
	{"Alert Notification Service", "org.bluetooth.service.alert_notification", 0x1811},
	{"Automation IO", "org.bluetooth.service.automation_io", 0x1815},
	{"Battery Service", "org.bluetooth.service.battery_service", 0x180F},
	{"Blood Pressure", "org.bluetooth.service.blood_pressure", 0x1810},
	{"Body Composition", "org.bluetooth.service.body_composition", 0x181B},
	{"Bond Management", "org.bluetooth.service.bond_management", 0x181E},
	{"Continuous Glucose Monitoring", "org.bluetooth.service.continuous_glucose_monitoring", 0x181F},
	{"Current Time Service", "org.bluetooth.service.current_time", 0x1805},
	{"Cycling Power", "org.bluetooth.service.cycling_power", 0x1818},
	{"Cycling Speed and Cadence", "org.bluetooth.service.cycling_speed_and_cadence", 0x1816},
	{"Device Information", "org.bluetooth.service.device_information", 0x180A},
	{"Environmental Sensing", "org.bluetooth.service.environmental_sensing", 0x181A},
	{"Generic Access", "org.bluetooth.service.generic_access", 0x1800},
	{"Generic Attribute", "org.bluetooth.service.generic_attribute", 0x1801},
	{"Glucose", "org.bluetooth.service.glucose", 0x1808},
	{"Health Thermometer", "org.bluetooth.service.health_thermometer", 0x1809},
	{"Heart Rate", "org.bluetooth.service.heart_rate", 0x180D},
	{"HTTP Proxy", "org.bluetooth.service.http_proxy", 0x1823},
	{"Human Interface Device", "org.bluetooth.service.human_interface_device", 0x1812},
	{"Immediate Alert", "org.bluetooth.service.immediate_alert", 0x1802},
	{"Indoor Positioning", "org.bluetooth.service.indoor_positioning", 0x1821},
	{"Internet Protocol Support", "org.bluetooth.service.internet_protocol_support", 0x1820},
	{"Link Loss", "org.bluetooth.service.link_loss", 0x1803},
	{"Location and Navigation", "org.bluetooth.service.location_and_navigation", 0x1819},
	{"Next DST Change Service", "org.bluetooth.service.next_dst_change", 0x1807},
	{"Object Transfer", "org.bluetooth.service.object_transfer", 0x1825},
	{"Phone Alert Status Service", "org.bluetooth.service.phone_alert_status", 0x180E},
	{"Pulse Oximeter", "org.bluetooth.service.pulse_oximeter", 0x1822},
	{"Reference Time Update Service", "org.bluetooth.service.reference_time_update", 0x1806},
	{"Running Speed and Cadence", "org.bluetooth.service.running_speed_and_cadence", 0x1814},
	{"Scan Parameters", "org.bluetooth.service.scan_parameters", 0x1813},
	{"Transport Discovery", "org.bluetooth.service.transport_discovery", 0x1824},
	{"Tx Power", "org.bluetooth.service.tx_power", 0x1804},
	{"User Data", "org.bluetooth.service.user_data", 0x181C},
	{"Weight Scale", "org.bluetooth.service.weight_scale", 0x181D},
#endif
	{"", "", 0}};

/**
 * @brief Convert an esp_ble_addr_type_t to a string representation.
 */
const char* BLEUtils::addressTypeToString(esp_ble_addr_type_t type) {
	switch (type) {
#if CONFIG_LOG_DEFAULT_LEVEL > 4
		case BLE_ADDR_TYPE_PUBLIC:
			return "BLE_ADDR_TYPE_PUBLIC";
		case BLE_ADDR_TYPE_RANDOM:
			return "BLE_ADDR_TYPE_RANDOM";
		case BLE_ADDR_TYPE_RPA_PUBLIC:
			return "BLE_ADDR_TYPE_RPA_PUBLIC";
		case BLE_ADDR_TYPE_RPA_RANDOM:
			return "BLE_ADDR_TYPE_RPA_RANDOM";
#endif
		default:
			return " esp_ble_addr_type_t";
	}
} // addressTypeToString

/**
 * @brief Convert the BLE Advertising Data flags to a string.
 * @param adFlags The flags to convert
 * @return std::string A string representation of the advertising flags.
 */
std::string BLEUtils::adFlagsToString(uint8_t adFlags)
{
	std::string res;
	if (adFlags & (1 << 0))
	{
		res += "[LE Limited Discoverable Mode] ";
	}
	if (adFlags & (1 << 1))
	{
		res += "[LE General Discoverable Mode] ";
	}
	if (adFlags & (1 << 2))
	{
		res += "[BR/EDR Not Supported] ";
	}
	if (adFlags & (1 << 3))
	{
		res += "[Simultaneous LE and BR/EDR to Same Device Capable (Controller)] ";
	}
	if (adFlags & (1 << 4))
	{
		res += "[Simultaneous LE and BR/EDR to Same Device Capable (Host)] ";
	}
	return res;
} // adFlagsToString

/**
 * @brief Given an advertising type, return a string representation of the type.
 *
 * For details see ...
 * https://www.bluetooth.com/specifications/assigned-numbers/generic-access-profile
 *
 * @return A string representation of the type.
 */
const char *BLEUtils::advTypeToString(uint8_t advType)
{
	switch (advType)
	{
#if CONFIG_LOG_DEFAULT_LEVEL > 4
	case GAP_ADTYPE_FLAGS: // 0x01
		return "GAP_ADTYPE_FLAGS";
	case GAP_ADTYPE_16BIT_MORE: // 0x02
		return "GAP_ADTYPE_16BIT_MORE";
	case GAP_ADTYPE_16BIT_COMPLETE: // 0x03
		return "GAP_ADTYPE_16BIT_COMPLETE";
	case GAP_ADTYPE_32BIT_MORE: // 0x04
		return "GAP_ADTYPE_32BIT_MORE";
	case GAP_ADTYPE_32BIT_COMPLETE: // 0x05
		return "GAP_ADTYPE_32BIT_COMPLETE";
	case GAP_ADTYPE_128BIT_MORE: // 0x06
		return "GAP_ADTYPE_128BIT_MORE";
	case GAP_ADTYPE_128BIT_COMPLETE: // 0x07
		return "GAP_ADTYPE_128BIT_COMPLETE";
	case GAP_ADTYPE_LOCAL_NAME_SHORT: // 0x08
		return "GAP_ADTYPE_LOCAL_NAME_SHORT";
	case GAP_ADTYPE_LOCAL_NAME_COMPLETE: // 0x09
		return "GAP_ADTYPE_LOCAL_NAME_COMPLETE";
	case GAP_ADTYPE_POWER_LEVEL: // 0x0a
		return "GAP_ADTYPE_POWER_LEVEL";
	case GAP_ADTYPE_OOB_CLASS_OF_DEVICE: // 0xd
		return "GAP_ADTYPE_OOB_CLASS_OF_DEVICE";
	case GAP_ADTYPE_OOB_SIMPLE_PAIRING_HASHC: // 0xe
		return "GAP_ADTYPE_OOB_SIMPLE_PAIRING_HASHC";
	case GAP_ADTYPE_OOB_SIMPLE_PAIRING_RANDR: // 0xf
		return "GAP_ADTYPE_OOB_SIMPLE_PAIRING_RANDR";
	case GAP_ADTYPE_SM_TK: // 0x10
		return "GAP_ADTYPE_SM_TK";
	case GAP_ADTYPE_SM_OOB_FLAG: // 0x11
		return "GAP_ADTYPE_SM_OOB_FLAG";
	case GAP_ADTYPE_INT_RANGE: // 0x12
		return "GAP_ADTYPE_INT_RANGE";
	case GAP_ADTYPE_SIGNED_DATA: // 0x13
		return "GAP_ADTYPE_SIGNED_DATA";
	case GAP_ADTYPE_SOL_BIT_UUID: // 0x14
		return "GAP_ADTYPE_SOL_BIT_UUID";
	case GAP_ADTYPE_128SOL_BIT_UUID: // 0x15
		return "GAP_ADTYPE_128SOL_BIT_UUID";
	case GAP_ADTYPE_SERVICE_DATA: // 0x16
		return "GAP_ADTYPE_SERVICE_DATA";
	case GAP_ADTYPE_PUBLIC_TARGET: // 0x17
		return "GAP_ADTYPE_PUBLIC_TARGET";
	case GAP_ADTYPE_RANDOM_TARGET: // 0x18
		return "GAP_ADTYPE_RANDOM_TARGET";
	case GAP_ADTYPE_APPEARANCE: // 0x19
		return "GAP_ADTYPE_APPEARANCE";
	case GAP_ADTYPE_ADV_INTERVAL: // 0x1a
		return "GAP_ADTYPE_ADV_INTERVAL";
	case GAP_ADTYPE_LE_BT_ADDR: // 0x1b
		return "GAP_ADTYPE_LE_BT_ADDR";
	case GAP_ADTYPE_LE_ROLE: // 0x1c
		return "GAP_ADTYPE_LE_ROLE";
	case GAP_ADTYPE_SP_HASH_C256: // 0x1d
		return "GAP_ADTYPE_SP_HASH_C256";
	case GAP_ADTYPE_SP_RAND_R256: // 0x1e
		return "GAP_ADTYPE_SP_RAND_R256";
	case GAP_ADTYPE_LIST_32BIT_SILI: //0x1F
		return "GAP_ADTYPE_LIST_32BIT_SILI";
	case GAP_ADTYPE_SERVICE_DATA_32BIT: // 0x20
		return "GAP_ADTYPE_SERVICE_DATA_32BIT";
	case GAP_ADTYPE_SERVICE_DATA_128BIT: // 0x21
		return "GAP_ADTYPE_128SERVICE_DATA";
	case GAP_ADTYPE_SC_CONF_VALUE: // 0x22
		return "GAP_ADTYPE_SC_CONF_VALUE";
	case GAP_ADTYPE_SC_RAND_VALUE: // 0x23
		return "GAP_ADTYPE_SC_RAND_VALUE";
	case GAP_ADTYPE_URI: // 0x24
		return "GAP_ADTYPE_URI";
	case GAP_ADTYPE_INDOOR_POSITION: // 0x25
		return "GAP_ADTYPE_INDOOR_POSITION";
	case GAP_ADTYPE_TRANSPORT_DISCOVERY_DATA: // 0x26
		return "GAP_ADTYPE_TRANSPORT_DISCOVERY_DATA";
	case GAP_ADTYPE_LE_SUPPORTED_FEATURES: // 0x27
		return "GAP_ADTYPE_LE_SUPPORTED_FEATURES";
	case GAP_ADTYPE_CHAN_MAP_UPDATE_IND: // 0x28
		return "GAP_ADTYPE_CHAN_MAP_UPDATE_IND";
	case GAP_ADTYPE_MESH_PB_ADV: // 0x29
		return "GAP_ADTYPE_MESH_PB_ADV";
	case GAP_ADTYPE_MESH_PACKET: // 0x2A
		return "GAP_ADTYPE_MESH_PACKET";
	case GAP_ADTYPE_MESH_BEACON: // 0x2B
		return "GAP_ADTYPE_MESH_BEACON";
	case GAP_ADTYPE_3D_INFO_DATA: // 0x3D
		return "GAP_ADTYPE_3D_INFO_DATA";
	case GAP_ADTYPE_MANUFACTURER_SPECIFIC: // 0xff
		return "ESP_BLE_AD_MANUFACTURER_SPECIFIC_TYPE";
#endif
	default:
		log_v(" adv data type: 0x%x", advType);
		return "";
	} // End switch
} // advTypeToString

/**
 * @brief Create a hex representation of data.
 *
 * @param [in] target Where to write the hex string.  If this is null, we malloc storage.
 * @param [in] source The start of the binary data.
 * @param [in] length The length of the data to convert.
 * @return A pointer to the formatted buffer.
 */
char* BLEUtils::buildHexData(uint8_t* target, uint8_t* source, uint8_t length) {
	// Guard against too much data.
	if (length > 100) length = 100;

	if (target == nullptr) {
		target = (uint8_t*) malloc(length * 2 + 1);
		if (target == nullptr) {
			log_e("buildHexData: malloc failed");
			return nullptr;
		}
	}
	char* startOfData = (char*) target;

	for (int i = 0; i < length; i++) {
		sprintf((char*) target, "%.2x", (char) *source);
		source++;
		target += 2;
	}

	// Handle the special case where there was no data.
	if (length == 0) {
		*startOfData = 0;
	}

	return startOfData;
} // buildHexData

/**
 * @brief Build a printable string of memory range.
 * Create a string representation of a piece of memory. Only printable characters will be included
 * while those that are not printable will be replaced with '.'.
 * @param [in] source Start of memory.
 * @param [in] length Length of memory.
 * @return A string representation of a piece of memory.
 */
std::string BLEUtils::buildPrintData(uint8_t* source, size_t length) {
	std::string res;
	for (int i = 0; i < length; i++) {
		char c = *source;
		res += (isprint(c) ? c : '.');
		source++;
	}
	return res;
} // buildPrintData
