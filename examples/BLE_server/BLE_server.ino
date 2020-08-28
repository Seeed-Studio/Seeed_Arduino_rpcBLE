#include "Seeed_erpcUnified.h"
#include "BLEUUID.h"
#include "BLEAddress.h"

void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
    };

    //Test BLEUUID
    Serial.println("Testing BLEUUID...");
    BLEUUID UUID16((uint16_t)0x180F);
    Serial.print("UUID16: ");
    Serial.println(UUID16.toString().c_str());
    BLEUUID UUID128("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
    Serial.print("UUID128: ");
    Serial.println(UUID128.toString().c_str());
    BLEUUID UUID128_1("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
    Serial.print("UUID128_1: ");
    Serial.println(UUID128_1.toString().c_str());
    if (UUID128.equals(UUID128_1))
    {
        Serial.println("UUID128 equals UUID128_1");
    }
    else
    {
        Serial.println("Error UUID128 not equals UUID128_1");
    }
    Serial.println("Test BLEUUID done\n\r");

    //Test BLEaddress
    Serial.println("Testing BLEaddress...");
    uint8_t address[6]{0x2c, 0x24, 0x25, 0x20, 0x24, 0xfc};

    BLEAddress Address1(address);
    Serial.print("Address1: ");
    Serial.println(Address1.toString().c_str());

    BLEAddress Address2("2c:24:25:20:24:fc");
    Serial.print("Address2: ");
    Serial.println(Address2.toString().c_str());

    if (Address1.equals(Address2))
    {
        Serial.println("Address1 equals Address2");
    }
    else
    {
        Serial.println("Error Address1 not equals Address2");
    }

    Serial.println("Testing BLEaddress done\n\r");
}

void loop()
{
}
