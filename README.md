<p align="center">
  <a href="https://github.com/tonimatutinovic/myDHT/releases">
    <img src="https://img.shields.io/github/v/release/tonimatutinovic/myDHT?label=Release">
  </a>
  <a href="https://www.ardu-badge.com/myDHT">
    <img src="https://www.ardu-badge.com/badge/myDHT.svg">
  </a>
  <a href="https://github.com/tonimatutinovic/myDHT/stargazers">
    <img src="https://img.shields.io/github/stars/tonimatutinovic/myDHT?style=social">
  </a>
  <a href="https://github.com/tonimatutinovic/myDHT/blob/main/LICENSE.txt">
    <img src="https://img.shields.io/badge/License-MIT-green.svg">
  </a>
  <img src="https://img.shields.io/badge/platform-Arduino-blue">
  <img src="https://img.shields.io/badge/language-C++-brightgreen">
</p>

<p align="center">
  <strong>Supports:</strong> DHT11, DHT22<br>
  <strong>Beginner Layer:</strong> <code>myDHT</code><br>
  <strong>Advanced Layer:</strong> <code>myDHTPro</code>
</p>

# myDHT – Layered, Zero-Dependency DHT Library for Arduino

> From absolute beginners to advanced users — one library, two layers, full control.

myDHT is a fully self-contained Arduino library for **DHT11** and **DHT22** temperature and humidity sensors,
designed to scale seamlessly from **first-time Arduino users** to **advanced embedded developers**.

Unlike most DHT libraries, myDHT does **not depend on any third-party drivers**.
The complete DHT protocol is implemented internally, including:

- start-signal generation  
- sensor acknowledgment handling  
- precise microsecond-level bit timing  
- checksum validation and sanity checks  

This makes myDHT suitable not only for reliable production use, but also for **educational purposes**
where understanding the underlying protocol and timing behavior is important.

---

## Why myDHT?

Most existing DHT libraries fall into one of two categories:

- **Beginner-friendly but opaque** — easy to use, but implemented as “black boxes.”  
- **Advanced but complex** — full control, but difficult to use correctly.

**myDHT solves this by design.**  
It offers **two layers**, so you choose the right balance between simplicity and control:

- **Beginner Layer (`myDHT`)** — simple, safe, and cached  
- **Advanced Layer (`myDHTPro`)** — full protocol access, debug, async, raw timing

Both layers share the **same core**, produce **identical results**, and require **zero external dependencies**.  
Start with `myDHT` and graduate to `myDHTPro` — no hardware changes required.

The layered approach is not an abstraction compromise — it is a deliberate design choice.

---

## Layered Architecture

### Architecture Overview

```
User Code
├── myDHT    # Beginner Layer: simple, safe, cached
└── myDHTPro # Advanced Layer: full control, debug, async
```

This design ensures beginners cannot misuse the sensor, while advanced users are not artificially limited.

Detailed capabilities of each layer are described in the **Features** section.

---

## Features

### Beginner Layer – `myDHT`

Designed for **simplicity, safety, and correctness**. Ideal for beginners, quick prototypes, and educational projects.

**You do not need to think about timing, retries, or protocol limitations.**


#### Sensor Support
- Full support for **DHT11** and **DHT22**
- Optional **automatic sensor type detection**
- Safe defaults for all timing and retry behavior

#### Measurements
- Temperature:
  - Celsius
  - Fahrenheit
  - Kelvin
- Relative humidity (%)
- Computed values:
  - Dew point (Magnus formula)
  - Heat index (Rothfusz regression)

#### Reliability & Safety
- Enforced minimum read interval (sensor-safe)
- Automatic internal caching
- Friendly error messages (optional)
- Invalid reads return `NaN` instead of stale data

#### Calibration
- Temperature offset calibration
- Humidity offset calibration

#### Beginner-Friendly API
- Simple Arduino-style functions:
  - `getTemperature()`
  - `getHumidity()`
  - `dewPoint()`
  - `HiIndex()`
- No manual timing management
- No protocol knowledge required

**Use this layer if you want correct readings with minimal code and zero surprises.**

---

### Advanced Layer – `myDHTPro`

Designed for **full control, transparency, and diagnostics**. Ideal for experienced users, research, debugging, and production-level monitoring

This layer makes the DHT protocol **observable, debuggable, and extensible**.

#### Full Protocol Implementation
- Manual start-signal generation
- Sensor acknowledgment handling
- Microsecond-level pulse timing measurement
- Checksum verification
- Automatic sensor type validation

#### Detailed Error Handling
- Explicit `DHTError` enum
- Last error tracking
- Consecutive failure counter
- Human-readable error strings
- Connection status helper (`isConnected()`)

#### Sanity Checks & Fail-Safe Mode
- Valid temperature range enforcement:
  - DHT11: 0–50 °C
  - DHT22: –40–80 °C
- Humidity range validation (0–100%)
- NaN detection
- Optional fallback to last known valid measurement

#### Raw Data Access
- Access to all 5 raw sensor bytes
- Access to raw high/low pulse durations for all 40 bits
- Ideal for debugging, analysis, and research

#### Unified Data API
- Single-call data acquisition:
  - temperature
  - humidity
  - dew point
  - heat index
  - status code

#### Asynchronous (Non-Blocking) Mode
- State-machine based async reads
- User callback on completion
- Non-blocking where physically possible  
  *(DHT protocol timing constraints still require a short blocking window)*

#### Debug & Test Modes
- Detailed debug output via Serial
- Internal state and timing diagnostics
- Test mode for validating sanity-check and fail-safe logic without hardware

#### Memory-Optimized Build
- Compile-time option to disable debug and test logic
- Reduced RAM usage
- Core functionality preserved

#### Multi-Sensor Support
- Manage multiple DHT11/DHT22 sensors simultaneously
- Centralized batch reading
- Individual result tracking (pin, values, error codes)
- Fully compatible with all advanced features

**Use this layer if you want to understand, debug, optimize, or extend DHT sensors.**

---

### Shared Core (Both Layers)

Both layers use the **same proven core**, producing identical measurements with zero external dependencies.  
Switching between layers requires **no hardware changes, wiring adjustments, or new library learning**.

---

## Code Examples

Below you can see how **myDHT** scales from **absolute beginner usage** to **advanced multi-sensor diagnostics**.  
The examples demonstrate the **simplicity**, **safety**, and **power** of each layer.
> Note: Beginner examples use the myDHT class, advanced examples use MyDHT (myDHTPro layer).

---

### Beginner Example – Full Features

- Easy to use for beginners  
- Friendly error messages if `enableFriendlyErrors(true)` is set  
- Only 4 lines of code to get reliable readings — beginners cannot go wrong
> Note: Beginner examples use the `myDHT` class (myDHT layer).

```cpp
#include <myDHT.h>   // Beginner layer

const int DHT_PIN = 2;
myDHT dht(DHT_PIN); // auto-detect sensor type

void setup() {
    Serial.begin(115200);
    dht.begin();  // Initialize sensor
}

void loop() {
    // Read temperature, humidity, dew point, heat index
    Serial.print("Temp: "); Serial.println(dht.getTemperature(Celsius));
    Serial.print("Hum: ");  Serial.println(dht.getHumidity());
    Serial.print("Dew: ");  Serial.println(dht.dewPoint(Celsius));
    Serial.print("HI: ");   Serial.println(dht.HiIndex(Celsius));
    
    delay(dht.getMinInterval()); // Ensures sensor-safe interval
}
```

#### Example output
```
Temp: 23.4°C
Hum: 45%
Dew: 9.2°C
HI: 24.0°C
```

> Note: Output may vary depending on sensor readings.

---

### Advanced Examples

The advanced layer (myDHTPro) exposes the full DHT protocol, debug, async reads, and multi-sensor support.

> Note: Advanced examples use the `MyDHT` class (myDHTPro layer).

#### Basic Reading

```cpp
#include <myDHTlib.h>

const int DHT_PIN = 2;
MyDHT dht(DHT_PIN, DHT_AUTO); // auto-detect DHT11 or DHT22

void setup() {
    Serial.begin(115200);
    dht.begin();
}

void loop() {
    DHTError err = dht.read();
    if (err == DHT_OK) {
        Serial.print("Temperature: "); Serial.println(dht.getTemperature(Celsius));
        Serial.print("Humidity: ");    Serial.println(dht.getHumidity());
        Serial.print("Dew Point: ");   Serial.println(dht.getDewPoint(Celsius));
        Serial.print("Heat Index: ");  Serial.println(dht.getHeatIndex(Celsius));
    } else {
        Serial.print("Read error: "); Serial.println(dht.getErrorString(err));
    }
    delay(2000);
}
```

---

#### Sanity-Check / Fail-Safe Demo (DHT_TEST_MODE)

```cpp
#include <myDHTlib.h>

MyDHT dht(2);
dht.testMode = true;  // Enables testing without real hardware

void setup() {
    Serial.begin(115200);
    dht.begin();
    Serial.println("Sanity-Check Demo");
}

void loop() {
    // Valid reading simulation
    dht.setRawBytes(0x1E, 0x00, 0x32, 0x00, 0x50); // T=30°C, H=50%
    DHTError err = dht.read();
    if (err == DHT_OK) {
        Serial.print("Temp: "); Serial.println(dht.getTemperature(Celsius));
        Serial.print("Hum: ");  Serial.println(dht.getHumidity());
    }

    delay(1000);

    // Invalid reading simulation
    dht.setRawBytes(0xFF,0xFF,0xFF,0xFF,0xFF);
    err = dht.read();
    if (err == DHT_ERROR_SANITY) {
        Serial.println("Invalid reading detected, fallback applied if possible");
    }
    delay(2000);
}
```

---

#### Debug Mode

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

---

#### Memory-Optimized Build

```cpp
#include <myDHTlib.h>

MyDHT dht(2);

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  DHTData data = dht.getData();  // Efficient reading
  if (data.status == DHT_OK) {
    Serial.print("Temp: "); Serial.println(data.temp);
    Serial.print("Hum: ");  Serial.println(data.hum);
  }
  delay(2000);
}
```

---

#### Error Handling

```cpp
DHTError err = dht.read();
dht.getLastError();        // Last error
dht.getFailureCount();     // Consecutive failures
dht.isConnected();         // True if sensor responded at least once
dht.getErrorString(err);   // Human-readable description
```

> Note: Friendly errors are optional in the beginner layer via enableFriendlyErrors(true).

---

#### Raw Data Access

```cpp
DHTRawData raw = dht.getRawData();
raw.bytes[0..4];      // Sensor bytes
raw.highTimes[0..39]; // High pulse durations (µs)
raw.lowTimes[0..39];  // Low pulse durations
```

---

#### Auto-Detection

```cpp
MyDHT dht(DHT_PIN, DHT_AUTO); // Library detects DHT11/DHT22
```

> Note: Retries automatically if detection fails, no user intervention required.

---

#### Multi-Sensor Manager

```cpp
// MultiSensorManager organizes multiple sensors efficiently without affecting read accuracy

#include <MultiSensorManager.h>
#include <myDHTlib.h>

MultiSensorManager manager(5);
MyDHT dht1(2, DHT11);
MyDHT dht2(3, DHT22);

void setup() {
    Serial.begin(115200);
    dht1.begin(); dht2.begin();
    manager.addSensor(dht1);
    manager.addSensor(dht2);
}

void loop() {
    manager.readAll();
    auto results = manager.getResults();
    for (int i=0; i<results.count; i++) {
        Serial.print("Pin "); Serial.print(results.data[i].pin); Serial.print(" → ");
        if (results.data[i].error == DHT_OK) {
            Serial.print(results.data[i].temperature);
            Serial.print("°C, "); Serial.println(results.data[i].humidity);
        } else {
            Serial.print("Error: "); Serial.println(results.data[i].error);
        }
    }
    delay(2000);
}
```

- Full set of examples available in the `examples/` folder for advanced usage.
- See documentation for calibration, async callbacks, and debug options.

---

## Tested Hardware

The **myDHT** library has been tested on real hardware to ensure correct timing, reliability, and protocol compliance.

### Officially Tested (Maintainer-Verified)

These configurations have been **fully tested by the library maintainer**, with all provided examples passing successfully.

| Board                      | MCU        | Clock  | Sensor | Status              |
|----------------------------|------------|--------|--------|---------------------|
| Arduino Uno                | ATmega328P | 16 MHz | DHT11  | All examples pass |
| Arduino Uno                | ATmega328P | 16 MHz | DHT22  | All examples pass |
| Arduino Nano (Lafvin clone)| ATmega328P | 16 MHz | DHT11  | All examples pass |
| Arduino Nano (Lafvin clone)| ATmega328P | 16 MHz | DHT22  | All examples pass |

These results confirm correct operation on **ATmega328P-based boards** using the standard **Arduino AVR core**.

---

### Smoke-Tested Hardware *(Planned)*

> This section will be populated after Arduino Library Manager propagation of v2.0.0.

---

### Community-Tested Hardware *(Planned)*

Boards and sensors confirmed by the community will be listed here once verified through issues, discussions, or pull requests.

> Community contributions are welcome and encouraged.

---

### Compatibility Notes

- Boards using **ATmega328P @ 16 MHz** are expected to work reliably  
- Nano-compatible clones are supported as long as they follow the standard reference design  

---

# 🏆 Why myDHT Is Better Than Standard DHT Libraries

**myDHT** is designed from the ground up to address limitations found in all existing DHT libraries.  
It is faster, safer, more accurate, and far more flexible.

### 1. Partially Non-Blocking Reads (Async State Machine)
Most DHT libraries freeze the CPU for up to **2 seconds** per reading.  
**myDHT uses a partially non-blocking state machine**, keeping the loop responsive while reading the sensor.  
> Note: The low-level 40-bit read must remain blocking for accurate timing, but all other operations are asynchronous.

Ideal for projects involving:
- LEDs, motors, displays
- WiFi or Bluetooth communication
- Multi-tasking applications

### 2. Multi-Sensor Support
Unlike standard libraries limited to one global sensor, **myDHT supports multiple sensors** via `MultiSensorManager` — perfect for weather stations or multi-room monitoring.

### 3. Automatic Sensor Detection
**myDHT auto-detects DHT11 vs DHT22**, removing user configuration errors common in other libraries.

### 4. Built-In Sanity Check & Fail-Safe Logic
Glitches, corrupted values, or unrealistic readings are rejected automatically:
- Invalid pulse timing
- Corrupted humidity/temperature combinations
- Out-of-range readings

### 5. Raw Pulse Debug Mode
Provides full transparency:
- High/low pulse timings
- Complete 40-bit raw bitstream
- Error cause reporting

Ideal for wiring troubleshooting and research.

### 6. Memory-Optimized Build
Optional compile-time flags reduce RAM usage, allowing deployment on memory-constrained MCUs (e.g., ATmega8, ATtiny).

### 7. Zero Dependencies / Minimal Overhead
**myDHT is fully standalone** — no external libraries or Arduino helper utilities are required.

### 8. Optimized DHT Engine
Bit-perfect implementation with precise timing ensures:
- Fewer checksum failures
- Fewer retries
- More accurate, consistent readings

---

## Feature Comparison

| Feature                 | myDHT | Standard DHT Lib | Adafruit DHT |
|-------------------------|-------|------------------|--------------|
| Async Read Capability   | Partial | No             | No           |
| Multiple Sensors        | Supported | Not supported | Not supported |
| Sensor Auto Detection   | Supported | Manual        | Manual       |
| Raw Protocol Access     | Available | Not available | Not available |
| Data Sanity Validation  | Full | Limited          | Limited      |
| Memory Optimization     | Available | Not available | Not available |


---

## Installation

You can install **myDHT** quickly using the **Arduino Library Manager** or manually.

### 1. Arduino Library Manager (Recommended)

1. Open the Arduino IDE.  
2. Navigate to **Sketch → Include Library → Manage Libraries…**  
3. Search for `myDHT`.  
4. Click **Install**.  

This is the easiest and most reliable way to keep the library up to date.

### 2. Manual Installation

1. Download the latest release from [GitHub Releases](https://github.com/tonimatutinovic/myDHT/releases).  
2. Unzip the folder.  
3. Move it into your Arduino libraries directory:  
```
Documents/Arduino/libraries/myDHT
```
4. Restart the Arduino IDE to load the library.  

### 3. Verify Installation

1. Open Arduino IDE → **Sketch → Include Library → myDHT**.  
2. Create a new sketch and include the library:  
```cpp
#include <myDHT.h>
```
3. Compile any example from the examples/ folder to confirm proper installation.

**Tip:** Restarting the Arduino IDE after installation ensures the library is detected correctly and prevents “library not found” errors.

---

## Folder structure
```
myDHT/
├── examples/
│   ├── BeginnerExamples/
│   │   ├── Starter/
│   │   │   └── Starter.ino
│   │   ├── TemperatureUnits/
│   │   │   └── TemperatureUnits.ino
│   │   ├── Offsets/
│   │   │   └── Offsets.ino
│   │   ├── DewPoint/
│   │   │   └── DewPoint.ino
│   │   ├── HeatIndex/
│   │   │   └── HeatIndex.ino
│   │   ├── FriendlyErrors/
│   │   │   └── FriendlyErrors.ino
│   │   └── MinIntervalCheck/
│   │       └── MinIntervalCheck.ino
│   └── AdvancedExamples/
│       ├── BasicRead/
│       │   └── BasicRead.ino
│       ├── CalibratedRead/
│       │   └── CalibratedRead.ino
│       ├── UnifiedRead/
│       │   └── UnifiedRead.ino
│       ├── RawRead/
│       │   └── RawRead.ino
│       ├── ErrorHandlingRead/
│       │   └── ErrorHandlingRead.ino
│       ├── AsyncRead/
│       │   └── AsyncRead.ino
│       ├── AutoDetect/
│       │   └── AutoDetect.ino
│       ├── SanityCheck/
│       │   └── SanityCheck.ino
│       ├── DebugMode/
│       │   └── DebugMode.ino
│       ├── MemoryOptimizedBuild/
│       │   └── MemoryOptimizedBuild.ino
│       └── MultiDHTManager/
│           └── MultiDHTManager.ino
├── src/
│   ├── beginner/
│   │   ├── myDHT.h
│   │   └── myDHT.cpp
│   └── advanced/
│       ├── myDHTPro.h
│       ├── myDHTPro.cpp
│       ├── myDHT_config.h
│       ├── MultiDHTManager.h
│       └── MultiDHTManager.cpp
├── keywords.txt
├── library.properties
├── LICENSE.txt
├── README.md
└── CHANGELOG.md

```

---

## Future Improvements

The **myDHT** library is actively evolving. Planned enhancements include:

- **Event-Based Callbacks**  
  Allow users to register callback functions that trigger automatically on new readings or specific threshold events, reducing the need for constant polling.

- **Moving Average / Smoothing**  
  Implement optional smoothing algorithms to reduce sensor noise and provide more stable temperature and humidity readings for sensitive applications.

- **Data History & Logging**  
  Enable lightweight internal storage of past readings, making it easier to track trends, debug issues, or implement custom analytics.

> These features will extend the library’s capabilities while keeping the core simplicity, safety, and performance intact.

---

## Contribute & Feedback
 
Your feedback and contributions help improve the library and benefit the entire Arduino community.

- Report issues or suggest features on [GitHub Issues](https://github.com/tonimatutinovic/myDHT/issues).  
- Contributions via pull requests are welcome.  
- Share your projects and results to inspire others.  

---
