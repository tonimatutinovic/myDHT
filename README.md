# MyDHT – Advanced Arduino Library for DHT11 and DHT22 Sensors

MyDHT is a self-contained, fully implemented Arduino driver for DHT11 and DHT22 temperature and humidity sensors.  
It does not rely on any third-party DHT libraries; instead, it manually implements the complete protocol, including start-signal generation, acknowledgment handling, and high-resolution bit-timing measurement.

This makes the library suitable for both practical use and educational purposes where full control and transparency are desired.

## Features

### Sensor Support
- Full support for **DHT11** and **DHT22**
- Optional **automatic sensor type detection**
- Configurable retry logic for handling unreliable sensors

### Measurements
- Temperature in Celsius, Fahrenheit, or Kelvin
- Relative humidity in percent
- Computed values:
  - Dew point (Magnus formula)
  - Heat index (Rothfusz regression)

### Reliability and Error Handling
- Detailed error reporting through the `DHTError` enum
- Last error tracking and error-to-string helper
- Consecutive failure counter
- Connection-status helper (`isConnected()`)

### Calibration and Validation
- Temperature and humidity offset calibration
- Internal clamping and sanity checking for out-of-range readings
- Optional fail-safe fallback to the last known valid measurement

### Raw Data Access
- Access to all 5 sensor bytes
- Access to raw high/low pulse durations for all 40 bits  
  Useful for debugging or research

### Single-Call Unified API
- `getData()` returns temperature, humidity, dew point, heat index, and status in one struct

### Asynchronous (Non-Blocking) Mode
- Optional state-machine-based asynchronous read
- User callback invoked upon completion
- **Note:** Full non-blocking behavior is not possible due to timing restrictions in the DHT protocol.  
  The library performs everything that *can* be asynchronous, but the low-level 40-bit read remains timing-critical and therefore executes in a short blocking window.

## Installation

Place the library folder into:
```
Documents/Arduino/libraries/MyDHT
```

## Basic Example

```cpp
#include <myDHTlib.h>

const int DHT_PIN = 2;

// Choose sensor manually:
// MyDHT dht(DHT_PIN, DHT22);
// Or let the library detect automatically:
MyDHT dht(DHT_PIN, DHT_AUTO);

void setup() {
    Serial.begin(115200);
    dht.begin();
}

void loop() {
    DHTError err = dht.read();

    if (err == DHT_OK) {
        Serial.print("Temperature: ");
        Serial.println(dht.getTemperature(Celsius));

        Serial.print("Humidity: ");
        Serial.println(dht.getHumidity());

        Serial.print("Dew Point: ");
        Serial.println(dht.getDewPoint(Celsius));

        Serial.print("Heat Index: ");
        Serial.println(dht.getHeatIndex(Celsius));
    }
    else {
        Serial.print("Read error: ");
        Serial.println(dht.getErrorString(err));
    }

    delay(2000);
}
```

## Error Handling
The library uses a clear and minimal error system:
- DHT_OK
- DHT_ERROR_NO_RESPONSE
- DHT_ERROR_TIMEOUT
- DHT_ERROR_CHECKSUM
- DHT_ERROR_BIT_TIMEOUT
- DHT_ERROR_INTERNAL
  
  Helpers:
  ```cpp
  dht.getLastError();       // Last error that occurred
  dht.getFailureCount();    // Number of consecutive failures
  dht.isConnected();        // True if the sensor has responded at least once
  dht.getErrorString(err);  // Human-readable error text
  ```

## Raw Data Access
```cpp
DHTRawData raw = dht.getRawData();
raw.bytes[0..4];      // Raw sensor bytes
raw.highTimes[0..39]; // High pulse durations in microseconds
raw.lowTimes[0..39];  // Low pulse durations
```

## Auto-Detection
If created with DHT_AUTO, the library attempts to determine whether the sensor is a DHT11 or DHT22 by analyzing returned data.
If detection fails, the sensor remains in DHT_AUTO mode and detection is retried later.

## Folder structure
```
MyDHT/
├── examples/
│   ├── BasicRead
│   ├── CalibratedRead
│   ├── UnifiedRead
│   ├── RawRead
│   ├── ErrorHandlingRead
│   ├── AsyncRead
│   └── AutoDetect
├── src/
|   ├── myDHTlib.h
|   └── myDHTlib.cpp
└── README.md
```

## Planned Improvements
Planned upgrades include:
- Fail-safe / Sanity Validation
- Debug Mode
- Memory-Optimized Build
- Multi-Sensor Support
