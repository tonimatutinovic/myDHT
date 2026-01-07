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
  <a href="https://github.com/tonimatutinovic/myDHT/blob/main/LICENSE">
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

Designed for **full control, transparency, and diagnostics**. Ideal for experienced users, research, debugging, and production-level monitoring.

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
#include <myDHTPro.h>

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
#include <myDHTPro.h>

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
#include <myDHTPro.h>

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
#include <myDHTPro.h>

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

The `MultiDHTManager` helper allows managing multiple DHT sensors
simultaneously with centralized reading and error tracking.

See the `10_MultiDHTManager` example in the `examples/Advanced/` folder
for a complete working demonstration.

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

### Smoke-Tested Hardware

> This section will be populated after wider community adoption of v2.0.2.

---

### Community-Tested Hardware

Boards and sensors confirmed by the community will be listed here once verified through issues, discussions, or pull requests.

> Community contributions are welcome and encouraged.

---

### Compatibility Notes

- Boards using **ATmega328P @ 16 MHz** are expected to work reliably  
- Nano-compatible clones are supported as long as they follow the standard reference design  

---
## Design Decisions & Key Advantages

**myDHT** is designed around a set of deliberate engineering decisions focused on
**reliability**, **predictable behavior**, and **clear separation of concerns**.

Rather than choosing between simplicity and control, myDHT provides **both** —
through a layered architecture that scales with user experience.

### Predictable Timing & Safe Sensor Access
DHT sensors are sensitive to timing and sampling intervals.  
myDHT enforces **sensor-safe minimum read intervals** and manages internal caching,
preventing accidental over-sampling and unstable readings.

### Layered API Design
- **Beginner layer (`myDHT`)** provides a safe, simplified API that prevents misuse.
- **Advanced layer (`myDHTPro`)** exposes full protocol access, diagnostics, and control.

Both layers share the same core implementation and produce identical results.

### Explicit Error Handling & Validation
Instead of silently returning incorrect values, myDHT:
- reports explicit error codes
- validates readings against realistic physical ranges
- tracks consecutive failures
- optionally falls back to the last known valid measurement

### Protocol Transparency
The advanced layer exposes raw sensor bytes, pulse timing, and detailed debug output,
making the DHT protocol observable and debuggable when needed.

### Zero Dependencies & Embedded-Friendly Design
myDHT is fully self-contained, with no external dependencies and predictable memory usage.
Optional compile-time optimizations allow deployment on resource-constrained MCUs.

In short, myDHT prioritizes **correctness, clarity, and robustness** over convenience shortcuts.

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
│   │   ├── 01_Starter/
│   │   │   └── 01_Starter.ino
│   │   ├── 02_TemperatureUnits/
│   │   │   └── 02_TemperatureUnits.ino
│   │   ├── 03_DewPoint/
│   │   │   └── 03_DewPoint.ino
│   │   ├── 04_HeatIndex/
│   │   │   └── 04_HeatIndex.ino
│   │   ├── 05_MinIntervalCheck/
│   │   │   └── 05_MinIntervalCheck.ino
│   │   ├── 06_Offsets/
│   │   │   └── 06_Offsets.ino
│   │   └── 07_FriendlyErrors/
│   │       └── 07_FriendlyErrors.ino
│   │
│   └── AdvancedExamples/
│       ├── 01_BasicRead/
│       │   └── 01_BasicRead.ino
│       ├── 02_AutoDetect/
│       │   └── 02_AutoDetect.ino
│       ├── 03_UnifiedRead/
│       │   └── 03_UnifiedRead.ino
│       ├── 04_CalibratedRead/
│       │   └── 04_CalibratedRead.ino
│       ├── 05_SanityCheck/
│       │   └── 05_SanityCheck.ino
│       ├── 06_ErrorHandlingRead/
│       │   └── 06_ErrorHandlingRead.ino
│       ├── 07_DebugMode/
│       │   └── 07_DebugMode.ino
│       ├── 08_RawRead/
│       │   └── 08_RawRead.ino
│       ├── 09_AsyncRead/
│       │   └── 09_AsyncRead.ino
│       ├── 10_MemoryOptimizedBuild/
│       │   └── 10_MemoryOptimizedBuild.ino
│       └── 11_MultiDHTManager/
│           └── 11_MultiDHTManager.ino
│
├── demo-projects/
│   └── beginner-demo-projects/
│       ├── 01_ZeroConfigWeatherMonitor/
│       │   ├── README.md
│       │   └── ZeroConfigWeatherMonitor/
│       │       └── ZeroConfigWeatherMonitor.ino
│       │
│       ├── 02_DewPointGreenhouseIndicator/
│       │   ├── README.md
│       │   ├── wiring.png
│       │   ├── images/
│       │   │   ├── demo02_greenhouse_indicator_green.jpg
│       │   │   └── demo02_greenhouse_indicator_red.jpg
│       │   └── DewPointGreenhouseIndicator/
│       │       └── DewPointGreenhouseIndicator.ino
│       │
│       └── 03_HeatIndexGauge/
│           ├── README.md
│           ├── wiring.png
│           ├── media/
│           │   ├── HeatIndexGauge_hero.jpg
│           │   ├── HeatIndexGauge_comfort.jpg
│           │   ├── HeatIndexGauge_warning.jpg
│           │   └── HeatIndexGauge_demo.mov
│           └── HeatIndexGauge/
│               └── HeatIndexGauge.ino
│
├── src/
│   ├── myDHT.h
│   ├── myDHT.cpp
│   ├── myDHTPro.h
│   ├── myDHTPro.cpp
│   ├── myDHT_config.h
│   ├── MultiDHTManager.h
│   └── MultiDHTManager.cpp
│
├── keywords.txt
├── library.properties
├── LICENSE
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
