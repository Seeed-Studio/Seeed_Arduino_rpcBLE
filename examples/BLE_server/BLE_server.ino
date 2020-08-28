#include "Seeed_erpcUnified.h"
#include "BLEUUID.h"

void setup()
{
    Serial.begin(115200);
    while(!Serial){};
    BLEUUID UUID16(0x180F);
    Serial.println(UUID16.toString().c_str());
    BLEUUID UUID128("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
    Serial.println(UUID128.toString().c_str());
}   

void loop()
{

}
