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

### Sanity-Check and Fail-Safe Mode
- Temperature must fall within valid ranges
  - DHT11: 0–50°C
  - DHT22: –40 to 80°C
- Humidity must be between 0–100%
- Values must not be NaN
- If a previous valid reading exists, last known good values are returned
- If no valid reading exists yet, error is returned normally

### Calibration and Validation
- Temperature and humidity offset calibration
- Internal clamping and sanity checking for out-of-range readings
- Optional fail-safe fallback to the last known valid measurement

### Raw Data Access
- Access to all 5 sensor bytes
- Access to raw high/low pulse durations for all 40 bits  
- Useful for debugging or research

### Single-Call Unified API
- `getData()` returns temperature, humidity, dew point, heat index, and status in one struct

### Asynchronous (Non-Blocking) Mode
- Optional state-machine-based asynchronous read
- User callback invoked upon completion
- **Note:** Full non-blocking behavior is not possible due to timing restrictions in the DHT protocol.  
  The library performs everything that *can* be asynchronous, but the low-level 40-bit read remains timing-critical and therefore executes in a short blocking window.

### Debug Mode
- Enables detailed debug messages via Serial
- Prints formatted internal state and read attempts
- Controlled via dht.debugMode = true;

### Memory-Optimized Build
- Reduces RAM usage by disabling Debug Mode and Test Mode
- Core reading, sanity check, and fail-safe functionality preserved
- Controlled at compile-time with constexpr bool optimizedBuild in library

## Installation

Place the library folder into:
```
Documents/Arduino/libraries/MyDHT
```

## Basic Example
Shows simple temperature and humidity readings.

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

## Sanity-Check / Fail-Safe Demo (DHT_TEST_MODE)

Use `testMode = true` to test fail-safe logic without real hardware.  
The library validates readings and optionally falls back to the last valid data.

```cpp
#include <myDHTlib.h>

MyDHT dht(2);
dht.testMode = true;

void setup() {
    Serial.begin(115200);
    dht.begin();
    Serial.println("Sanity-Check Demo");
}

void loop() {
    // --- Valid reading ---
    dht.setRawBytes(0x1E, 0x00, 0x32, 0x00, 0x50); // T=30°C, H=50%
    DHTError err = dht.read();

    if (err == DHT_OK) {
        Serial.print("Temp: "); Serial.println(dht.getTemperature(Celsius));
        Serial.print("Hum: ");  Serial.println(dht.getHumidity());
    }

    delay(1000);

    // --- Invalid reading ---
    dht.setRawBytes(0xFF,0xFF,0xFF,0xFF,0xFF);
    err = dht.read();

    if (err == DHT_ERROR_SANITY) {
        Serial.println("Invalid reading detected, fallback applied if possible");
    }

    delay(2000);
}
```

## Debug Mode
Demonstrates how to enable debug prints for troubleshooting sensor reads.
```cpp
#include <myDHTlib.h>

MyDHT dht(2);

void setup() {
  Serial.begin(115200);
  dht.debugMode = true;  // Enable debug prints
  dht.begin();
}

void loop() {
  dht.read();
  delay(2000);
}
```

## Memory-Optimized Build
Shows usage of the lightweight memory build, which skips debug and test logic.
```cpp
#include <myDHTlib.h>

MyDHT dht(2);

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  DHTData data = dht.getData();  // Optimized build reads sensor efficiently
  if (data.status == DHT_OK) {
    Serial.print("Temp: "); Serial.println(data.temp);
    Serial.print("Hum: "); Serial.println(data.hum);
  }
  delay(2000);
}
```
---
All other examples, such as calibrated readings, raw data access, error handling, async reads are available in the examples/ folder.

---
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
│   ├── AutoDetect
│   └── SanityCheck
├── src/
|   ├── myDHTlib.h
|   └── myDHTlib.cpp
└── README.md
```

## Planned Improvements
- Multi-Sensor Support