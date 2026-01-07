# Changelog

All notable changes to this project will be documented in this file.

The format is based on Keep a Changelog,
and this project adheres to Semantic Versioning.

---
## [2.0.2] – 07.01.2026.

### Added
- **Beginner demo projects**
  - Zero-Config Weather Monitor
  - Dew-Point Greenhouse Indicator
  - Heat Index Gauge
- Wiring diagrams, images, and media assets for beginner demo projects
- `readSafe()` in **myDHTPro** to enforce sensor-safe minimum interval between reads

### Changed
- Updated minimum read interval behavior to better match real-world DHT11/DHT22 sampling reliability
- Improved **memory-optimized build** flow (clearer behavior when debug/test features are disabled at compile time)
- Reorganized `examples/` folder for improved Arduino IDE discoverability
- Standardized example naming and ordering for beginner and advanced layers
- Updated README with:
  - clearer layered architecture explanation
  - refined design rationale
  - complete repository, examples, and demo-project structure

### Fixed
- Read-interval edge cases that could lead to unstable/`NaN` readings when polled too quickly
- Minor documentation inconsistencies and outdated references (examples, file names, folder structure)

---
## [2.0.0] – 15.12.2025.

### Added
- **Beginner layer (`myDHT`)**
  - Simple, Adafruit-like API for beginners
  - Automatic sensor type detection (`DHT_AUTO`)
  - Single-call functions:
    - `getTemperature()`
    - `getHumidity()`
    - `dewPoint()`
    - `HiIndex()`
  - Friendly error messages via Serial
  - Internal read caching with automatic minimum interval handling
  - `getMinInterval()` helper for user-side timing
  - Temperature and humidity offset calibration

- **Advanced layer (`myDHTPro`)**
  - Full manual DHT protocol implementation (no third-party dependencies)
  - Asynchronous (non-blocking) read mode with callback
  - Unified data access via `getData()`
  - Raw data access (bytes and pulse timings)
  - Sanity-check and fail-safe fallback mode
  - Debug mode with detailed Serial output
  - Memory-optimized build option
  - Multi-sensor support via `myDHTManager`

- **Examples**
  - Beginner examples covering all beginner-layer features
  - Clear separation between beginner and advanced examples

- **Documentation**
  - Updated README with beginner/advanced layer explanation
  - Folder structure reorganized for clarity
  - Added list of tested boards and sensors
  - Added this CHANGELOG

---

### Changed
- Library architecture split into **Beginner (`myDHT`)** and **Advanced (`myDHTPro`)** layers
- Improved internal read efficiency using smart caching
- Unified naming and API consistency across layers
- Examples standardized by layer and baud rate

---

### Fixed
- Improved handling of disconnected sensors
- Eliminated reuse of stale data in beginner layer on failed reads
- Corrected minimum read interval handling for DHT11 and DHT22
- Improved error reporting consistency

---

### Removed
- Legacy single-layer API
- Implicit access to last valid data in beginner layer
- Ambiguous example naming and structure

---

## [1.0.0] – Initial Release

- Initial implementation of DHT11 and DHT22 support
- Manual protocol implementation