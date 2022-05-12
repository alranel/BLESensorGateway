/**
   See https://github.com/ChrisScheffler/miflora/wiki/The-Basics for details on the protocol.
*/

#include <rduinoBLE.h>
#include <functional>
#include <map>

struct MiFlora_data_t {
  float temperature;
  int moisture, light, conductivity, battery;
};

typedef std::function<void(String,MiFlora_data_t)> MiFlora_callback_t;

std::function<bool(BLEDevice&)> MiFlora_handler(MiFlora_callback_t cb) {
  return [cb](BLEDevice& device) {
    // connect to the peripheral
    Serial.println("Connecting ...");
    if (!device.connect()) {
      Serial.println("Failed to connect!");
      return false;
    }
    Serial.println("Connected");

    // discover peripheral attributes
    Serial.println("Discovering service...");
    if (!device.discoverService("00001204-0000-1000-8000-00805f9b34fb")) {
      Serial.println("Attribute discovery failed.");
      device.disconnect();
      return false;
    }
    Serial.println("Service discovered");

    // forcing device in data mode
    {
      BLECharacteristic writeModeCharacteristic = device.characteristic("00001a00-0000-1000-8000-00805f9b34fb");
      if (!writeModeCharacteristic) {
        Serial.println("Failed to get write mode characteristic");
        return false;
      }
      uint8_t buf[2] = {0xA0, 0x1F};
      writeModeCharacteristic.writeValue(buf, 2, true);
      delay(500);    
    }

    // read data
    MiFlora_data_t data;

    {
      BLECharacteristic dataCharacteristic = device.characteristic("00001a01-0000-1000-8000-00805f9b34fb");
      if (!dataCharacteristic) {
        Serial.println("Failed to get sensor data characteristic");
        return false;
      }

      char val[16];
      dataCharacteristic.readValue(val, sizeof(val));

      Serial.print("Hex: ");
      for (int i = 0; i < 16; i++) {
        Serial.print((int)val[i], HEX);
        Serial.print(" ");
      }
      Serial.println(" ");

      const int16_t* temp_raw = (int16_t*)val;
      data.temperature = (*temp_raw) / ((float)10.0);
      Serial.print("-- Temperature: ");
      Serial.println(data.temperature);

      data.moisture = val[7];
      Serial.print("-- Moisture: ");
      Serial.println(data.moisture);

      data.light = val[3] + val[4] * 256;
      Serial.print("-- Light: ");
      Serial.println(data.light);
    
      data.conductivity = val[8] + val[9] * 256;
      Serial.print("-- Conductivity: ");
      Serial.println(data.conductivity);

      if (data.temperature > 200) {
        Serial.println("-- Unreasonable values received, skip publish");
        return false;
      }
    }

    // read battery data
    {
      BLECharacteristic batteryCharacteristic = device.characteristic("00001a02-0000-1000-8000-00805f9b34fb");
      if (!batteryCharacteristic) {
        Serial.println("Failed to get sensor data characteristic");
        return false;
      }

      char val[1];
      batteryCharacteristic.readValue(val, 1);

      data.battery = val[0];
      Serial.print("-- Battery: ");
      Serial.println(data.battery);    
    }

    device.disconnect();
    delay(2000);

    cb(device.address(), data);

    return true;
  };
};

