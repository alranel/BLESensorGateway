/**
   A BLE client for the Xiaomi Mi Plant Sensor, pushing measurements to Arduino Cloud.
*/

#include "arduino_secrets.h"
#include "thingProperties.h"
#include <BLESensorGateway.h>
#include <MiFlora.h>

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  // Initialize cloud variables, but don't connect
  initProperties();
  setDebugMessageLevel(4);

  // Add one or more BLE devices providing a callback that receives data and updates cloud variables
  BLESensorGateway.add("c4:7c:8d:6d:45:28", MiFlora_handler(callback));

  // Read data from BLE every 30 minutes
  BLESensorGateway.setInterval(20);
  
  // Start the gateway
  BLESensorGateway.begin(ArduinoIoTPreferredConnection);
}

void loop() {
  BLESensorGateway.update();
}

void callback(String deviceAddress, MiFlora_data_t data) {
  temperature  = data.temperature;
  moisture     = data.moisture;
  light        = data.light;
  conductivity = data.conductivity;
  battery      = data.battery;
}
