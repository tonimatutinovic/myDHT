# 📘 MyDHT — Minimal Arduino DHT11 Library

**MyDHT** is a simple and educational Arduino library for the **DHT11** temperature and humidity sensor.  
The library implements the core communication protocol manually (start signal, ACK, bit timing), without relying on existing DHT libraries — making it ideal for learning low-level sensor communication and building custom drivers.

This is the initial version of the library, and the project will be expanded and improved over time.

---

## ✨ Current Features

- Support for **DHT11**
- Manual implementation of the DHT communication protocol  
- Temperature and humidity reading  
- Checksum validation  
- Simple integration into Arduino projects

---

## 📂 Repository Structure

```
MyDHT/
├── examples/
│  └── BasicRead/BasicRead.ino
├── src/
│  ├── myDHTlib.h
│  └── myDHTlib.cpp
└── README.md
```

---

## 📝 Basic Usage Example

```cpp
#include <myDHTlib.h>

const int DHT_PIN = 2;
MyDHT dht(DHT_PIN);

void setup() {
  Serial.begin(115200);
  dht.begin();
  delay(2000);
}

void loop() {
  if (dht.read()) {
    Serial.print("Temperature: ");
    Serial.println(dht.getTemperature());

    Serial.print("Humidity: ");
    Serial.println(dht.getHumidity());
  } else {
    Serial.println("Error reading from sensor!");
  }

  delay(2000);
}
```

## 🚧 Planned Improvements
 
Planned upgrades include:

- Support for **DHT22**
- Retry logic for more reliable readings
