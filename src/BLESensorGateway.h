#include <rduinoBLE.h>
#include <Arduino_ConnectionHandler.h>
#include <ArduinoIoTCloud.h>
#include <WiFiNINA.h>
#include <functional>
#include <map>

class BLESensorGatewayClass {
  public:
  void add(const char* mac, std::function<bool(BLEDevice&)> handler) {
    _devices[mac] = handler;
  };
  void setInterval(int i) {
    _interval = i*1000;
  };
  void begin(ConnectionHandler& conn);
  void update();

  private:
  enum class State {
    NONE,
    BLE,  // reading from BLE devices
    WIFI, // connected to cloud
  };
  State _state = State::NONE;
  ConnectionHandler* _conn;
  std::map<String,std::function<bool(BLEDevice&)>> _devices;
  int _seenCount;
  unsigned long _interval = 1800*1000; // milliseconds
  unsigned long _startedWiFi;
  void _beginBLE();
  void _beginWiFi();
};

extern BLESensorGatewayClass BLESensorGateway;
