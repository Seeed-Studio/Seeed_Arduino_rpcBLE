Introduction
Seeed Arduino rpcBLE library is support to Wio terminal BLE function. 

How to install Arduino Library 

please review [**here**](https://wiki.seeedstudio.com/How_to_install_Arduino_Library/)

we are provide some BLE example below:

- [**BLE_client**](https://github.com/Seeed-Studio/Seeed_Arduino_rpcBLE/tree/master/examples/BLE_client)
- [**BLE_iBeacon**](https://github.com/Seeed-Studio/Seeed_Arduino_rpcBLE/tree/master/examples/BLE_iBeacon)
- [**BLE_scan**](https://github.com/Seeed-Studio/Seeed_Arduino_rpcBLE/tree/master/examples/BLE_scan)
- [**BLE_server**](https://github.com/Seeed-Studio/Seeed_Arduino_rpcBLE/tree/master/examples/BLE_server)
- [**BLE_uart**](https://github.com/Seeed-Studio/Seeed_Arduino_rpcBLE/tree/master/examples/BLE_uart)
- [**BLE_XYZ_iBeacon**](https://github.com/Seeed-Studio/Seeed_Arduino_rpcBLE/tree/master/examples/BLE_iBeacon_3Asix)

There are some main API reference

BLE initialization：
```CPP
BLEDevice::init("");
```

BLE devices scan function：
```CPP
BLEDevice::getScan();
```

bluetooth mode: ture is mean Active Scanning, false is mean Passive Scanning
```CPP
BLEScan::setActiveScan(true);
```

enable scan:
```CPP
BLEScan::start();
```

disable scan:
```CPP
BLEScan::stop();
```

create new client:
```CPP
BLEDevice::createClient()
```

Bluetooth connect：
```CPP
BLEClient::connect();
```

Bluetooth disconnect：
```CPP
BLEClient::disconnect();
```

create new server：
```CPP
BLEDevice::createServer();
```

Create a service for server：
```CPP
BLEServer::createService();
```

Create a Characteristic for service：
```CPP
BLEService::createCharacteristic()
```

Create a Descriptor for Characteristic：
```CPP
BLECharacteristic::createDescriptor()
```

Create Advertising data：
```CPP
BLEDevice::getAdvertising();
```
Set the advertisement data that is to be published in a scan response：
```CPP
BLEAdvertising::setScanResponse(true);
```


License

This software is written by seeed studio
and is licensed under The MIT License. Check License.txt for more information.

Contributing to this software is warmly welcomed. You can do this basically by
forking, committing modifications and then pulling requests (follow the links above
for operating guide). Adding change log and your contact into file header is encouraged.
Thanks for your contribution.

Seeed Studio is an open hardware facilitation company based in Shenzhen, China.
Benefiting from local manufacture power and convenient global logistic system,
we integrate resources to serve new era of innovation. Seeed also works with
global distributors and partners to push open hardware movement.






















