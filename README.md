# BLESensorGateway

[![License](https://img.shields.io/github/license/alranel/BLESensorGateway)](https://support.arduino.cc/hc/en-us/articles/360018434279-I-have-used-Arduino-for-my-project-do-I-need-to-release-my-source-code-)

This library enables the creation of bidirectional gateways that talk to local BLE devices and relay their data to [Arduino Cloud](https://cloud.arduino.cc/). For instance, you can use it to read data from a BLE moisture sensor and send it to the cloud.

**_Does it support the reverse flow (i.e. control a BLE actuator based on cloud events)?_**

Yes, but maybe it's not fully optimized for that flow and could result in slow cycles. If you have a specific use case, please open an issue describing your BLE device and scenario and we'll figure out how to improve it together.

**_How does it handle BLE and WiFi at the same time?_**

Unfortunately, Arduino boards based on the NINA module cannot talk to BLE and WiFi at the same time, so this library **switches automatically between the two modes** according to a configurable time interval.

**_What BLE devices does it support?_**

This library is implemented as an abstract framework so that you can write the logic to talk to any specific BLE device. The library will take care of everything else: you just need to write the commands that read data from your sensor. As an example, **the implementation for the Xiaomi Mi Flora sensor is provided**. If you want to submit more implementations and make them available to other users, just open a pull request in this repository and I'll happily merge them.

**_How do I use the provided example?_**

Configure your board and your variables on [Arduino IoT Cloud](https://cloud.arduino.cc). Then, within the IoT Cloud interface, click on "Open Full Editor" and you'll get the web-based Arduino editor. From there, copy all the sketch files to your online sketch and upload it. Make sure you enter your WiFi credentials.

If you prefer to use the desktop Arduino IDE instead, just replace the thingProperties.h file in the example sketch with the auto-generated thingProperties.h file from the Web Editor. Then rename arduino_secrets.h-dist to arduino_secrets.h and put your WiFi credentials there.

> See the [library home page](https://www.arduino.cc/reference/en/libraries/blesensorgateway/) and the [examples](examples/) for more information about this library and its usage.

## Usage

```arduino
#include "arduino_secrets.h"
#include "thingProperties.h"
#include <BLESensorGateway.h>

void setup() {
  Serial.begin(115200);
  delay(1500);

  // Initialize cloud variables, but don't connect
  initProperties();
  setDebugMessageLevel(4);

  // Add one or more BLE devices
  BLESensorGateway.add("c4:7c:8d:6d:45:28", my_handler);

  // Read data from BLE every 30 minutes
  BLESensorGateway.setInterval(30*60);
  
  // Start the gateway
  BLESensorGateway.begin(ArduinoIoTPreferredConnection);
}

void loop() {
  BLESensorGateway.update();
}

bool my_handler(BLEDevice& device) {
    // ... read data from the device and assign it to the cloud variable
    my_cloud_var = xxx;
}
```

## 🔎 Resources

* [How to install a library](https://www.arduino.cc/en/guide/libraries)
* [Help Center](https://support.arduino.cc/)
* [Forum](https://forum.arduino.cc)

## 🐛 Bugs & Issues

If you want to report an issue with this library, you can submit it to the [issue tracker](issues) of this repository. Remember to include as much detail as you can about your hardware set-up, code and steps for reproducing the issue.

## 👨‍💻 Development

There are many ways to contribute:

* Improve documentation and examples
* Fix a bug
* Test open Pull Requests
* Implement a new feature
* Discuss potential ways to improve this library

You can submit your patches directly to this repository as Pull Requests. Please provide a detailed description of the problem you're trying to solve and make sure you test on real hardware.

## Credits

This library was developed by Alessandro Ranellucci.
