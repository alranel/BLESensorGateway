#include "BLESensorGateway.h"

void BLESensorGatewayClass::begin(ConnectionHandler& conn) {
  _conn = &conn;
  _beginBLE();
}

void BLESensorGatewayClass::update() {
  if (_state == State::BLE) {
    BLEDevice device = BLE.available();
    if (device) {
      // discovered a peripheral, print out address, local name, and advertised service
      Serial.print("Found ");
      Serial.print(device.address());
      Serial.print(" '");
      Serial.print(device.localName());
      Serial.print("' ");
      Serial.print(device.advertisedServiceUuid());
      Serial.println();

      // Check if this is a known device
      auto d = _devices.find(device.address().c_str());
      if (d != _devices.end()) {
        // Device is known. Get data.
        BLE.stopScan();
        bool res = d->second(device);
        if (res) {
          _seenCount++;
          if (_seenCount == _devices.size()) {
            _beginWiFi();
            return;
          }
        }
        BLE.scan();
      }
    }
  } else if (_state == State::WIFI) {
    if (millis() - _startedWiFi > _interval) {
      _beginBLE();
      return;
    }
    ArduinoCloud.update();
  }
}

void BLESensorGatewayClass::_beginBLE() {
  if (_state == State::BLE) return;
  if (_state == State::WIFI) {
    _conn->disconnect();
  }

  Serial.println("Initializing BLE client...");
  if (!BLE.begin()) {
    Serial.println("BLE initialization failed!");
    while (1);
  }

  // start scanning for peripheral
  _seenCount = 0;
  BLE.scan();

  _state = State::BLE;
}

void BLESensorGatewayClass::_beginWiFi() {
  if (_state == State::WIFI) return;
  if (_state == State::BLE) {
    BLE.stopScan();
    BLE.end();
    wiFiDrv.wifiDriverDeinit();
    wiFiDrv.wifiDriverInit();
  }

  Serial.println("Connecting to WiFi...");
  ArduinoCloud.begin(*_conn);
  ArduinoCloud.printDebugInfo();

  _state = State::WIFI;
  _startedWiFi = millis();
}

BLESensorGatewayClass BLESensorGateway;
