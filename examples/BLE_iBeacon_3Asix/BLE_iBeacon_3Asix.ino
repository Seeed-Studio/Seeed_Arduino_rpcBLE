/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by pcbreflux
*/

 
/*
   Create a BLE server that will send periodic iBeacon frames.
   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create advertising data
   3. Start advertising.
   4. wait
   5. Stop advertising.
   6. deep sleep
   
*/


#include "sys/time.h"
#include "rpcBLEDevice.h"
#include "BLEBeacon.h"
#include"LIS3DHTR.h"

#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
TFT_eSprite spr = TFT_eSprite(&tft);  // Sprite 

LIS3DHTR<TwoWire> lis;
BLEAdvertising *pAdvertising;


#define BEACON_UUID           "8ec76ea3-6668-48da-9866-75be8bc86f4d" // UUID 1 128-Bit (may use linux tool uuidgen or random numbers via https://www.uuidgenerator.net/)
#define SERVICE_UUID           0x1801
#define SERVICE_UUID1          0x1802
#define SERVICE_UUID2          0x1803

  
void setBeacon() {

  BLEBeacon oBeacon = BLEBeacon();
  oBeacon.setManufacturerId(0x4C00); // fake Apple 0x004C LSB (ENDIAN_CHANGE_U16!)
  oBeacon.setProximityUUID(BLEUUID(BEACON_UUID));
  oBeacon.setMajor(0x007B);
  oBeacon.setMinor(0x01C8);
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  BLEAdvertisementData oScanResponseData = BLEAdvertisementData();
  
  oAdvertisementData.setFlags(0x04); // BR_EDR_NOT_SUPPORTED 0x04
  std::string strServiceData = "";
  strServiceData += (char)26;     // Len
  strServiceData += (char)0xFF;   // Type
  strServiceData += oBeacon.getData(); 
  oAdvertisementData.addData(strServiceData);

  union cracked_float_t{
  float f;
  uint32_t l;
  word w[sizeof(float)/sizeof(word)];
  byte b[sizeof(float)];
};

  float x_values, y_values, z_values;
  x_values = lis.getAccelerationX();
  y_values = lis.getAccelerationY();
  z_values = lis.getAccelerationZ();

  std::string m_data = std::string((char*) &x_values, sizeof(x_values));
  std::string m_data1 = std::string((char*) &y_values, sizeof(y_values));
  std::string m_data2 = std::string((char*) &z_values, sizeof(z_values));
  oScanResponseData.setName("wio");
  oScanResponseData.setServiceData(BLEUUID(SERVICE_UUID),m_data);
  oScanResponseData.setServiceData(BLEUUID(SERVICE_UUID1),m_data1);
  oScanResponseData.setServiceData(BLEUUID(SERVICE_UUID2),m_data2);

  pAdvertising->setAdvertisementData(oAdvertisementData);
  pAdvertising->setScanResponseData(oScanResponseData);
  pAdvertising->setAdvertisementType(GAP_ADTYPE_ADV_SCAN_IND);



  cracked_float_t X = {x_values};
  cracked_float_t Y = {y_values};
  cracked_float_t Z = {z_values};

  spr.fillSprite(TFT_BLACK);
  spr.createSprite(240, 160);
  spr.fillSprite(TFT_BLACK);
  spr.setTextColor(TFT_WHITE, TFT_BLACK);
  spr.setFreeFont(&FreeSansBoldOblique12pt7b);

  uint8_t x_values_b [4];
  memcpy(x_values_b, &x_values, 4);
  char x[10];
  sprintf(x, "%02X%02X%02X%02X", x_values_b [0],x_values_b [1],x_values_b [2],x_values_b [3]);

  uint8_t y_values_b [4];
  memcpy(y_values_b, &y_values, 4);
  char y[10];
  sprintf(y, "%02X%02X%02X%02X", y_values_b [0],y_values_b [1],y_values_b [2],y_values_b [3]);
  
  uint8_t z_values_b [4];
  memcpy(z_values_b, &z_values, 4);
  char z[10];
  sprintf(z, "%02X%02X%02X%02X", z_values_b [0],z_values_b [1],z_values_b [2],z_values_b [3]);
  
    
  spr.drawString("X- 0x",20 ,10); 
  spr.drawString(x, 80, 10);
  spr.drawString("X-value",20 ,35); 
  spr.drawFloat(x_values, 2, 120, 35);
  
  spr.drawString("Y- 0x",20 , 60); 
  spr.drawString(y, 80, 60);
  spr.drawString("Y-value",20 ,85); 
  spr.drawFloat(y_values, 2, 120, 85);
  
  spr.drawString("Z- 0x",20 , 110); 
  spr.drawString(z, 80, 110);
  spr.drawString("Z-value",20 ,135); 
  spr.drawFloat(z_values, 2, 120, 135);

  spr.pushSprite(0,0);
  

}

void setup() {

  tft.begin();
  tft.init();
  tft.setRotation(3);  
  tft.fillScreen(TFT_BLACK);  
  Serial.begin(115200);
  
  // Create the BLE Device
  BLEDevice::init("");
  lis.begin(Wire1);
  if (!lis) {
    Serial.println("ERROR");
    while(1);
  }
  lis.setOutputDataRate(LIS3DHTR_DATARATE_25HZ); //Data output rate
  lis.setFullScaleRange(LIS3DHTR_RANGE_2G); //Scale range set to 2g

  
  pAdvertising = BLEDevice::getAdvertising();
  
  
  
  Serial.println("Advertizing started...");
  delay(100);
}

void loop() {
  setBeacon();
  // Start advertising
  pAdvertising->start();
  delay(5000);
  pAdvertising->stop();
  delay(1000);
}
