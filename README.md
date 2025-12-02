# 📘 MyDHT — Advanced Arduino DHT11/DHT22 Library

**MyDHT** is an advanced, lightweight, and fully self-implemented Arduino library for **DHT11** and **DHT22** temperature/humidity sensors.

Unlike typical DHT libraries, **MyDHT manually implements the entire low-level protocol**  
(start signal → ACK → bit-timing decoding) without using any existing libraries.  
This makes it ideal for:

🟦 Learning how DHT sensors work on the signal level  
🟦 Building custom or optimized sensor drivers  
🟦 Having full control over retries, timing, conversion units, and derived metrics  

---

## ✨ Current Features

### ✔ Core Sensor Support
- Full support for **DHT11**
- Full support for **DHT22**
- Manual protocol decoding (no third-party dependencies)
- Selectable retry count (`setRetries()`)

### ✔ Temperature & Humidity
- Temperature in:
  - **Celsius**
  - **Fahrenheit**
  - **Kelvin**
- Humidity in %

### ✔ Derived Metrics (Computed Values)
- 🌡 **Dew Point**
- 🔥 **Heat Index**  
- Automatically adapts depending on temperature unit  
- Correct formulas for both °C and °F

### ✔ Configuration
- Internal temperature unit mode (`_unit`)  
- Automatically set by `getTemperatureC/F/K()`  
- Default unit chosen based on global user distribution (Celsius by default)

### ✔ Error Handling
Detailed error codes returned by `read()`:
- `DHT_OK`
- `DHT_NO_RESPONSE`
- `DHT_ACK_TIMEOUT`
- `DHT_CHECKSUM_FAIL`
- `DHT_BIT_TIMEOUT`

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

// For DHT11:
MyDHT dht(DHT_PIN, DHT11);

// For DHT22:
// MyDHT dht(DHT_PIN, DHT22);

void setup() {
  Serial.begin(115200);
  dht.begin();
  delay(2000);
}

void loop() {
  DHTError err = dht.read();

  if (err == DHT_OK) {
    Serial.print("Temperature (C): ");
    Serial.println(dht.getTemperatureC());

    Serial.print("Humidity: ");
    Serial.println(dht.getHumidity());

    Serial.print("Dew Point: ");
    Serial.println(dht.getDewPoint());

    Serial.print("Heat Index: ");
    Serial.println(dht.getHeatIndex());
  } else {
    Serial.print("Read error: ");
    Serial.println(err);
  }

  delay(2000);
}
```

## 🚧 Planned Improvements
 
Planned upgrades include:

- Temperature + humidity calibration support
- Raw data access (raw bytes + raw bit timing)
- getData() struct API (single call returning all values)
- Async / non-blocking read mode
- Auto-detect DHT11 / DHT22
