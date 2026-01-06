# Demo 01 – Zero-Config Weather Monitor

This beginner demo shows the **simplest and safest way** to read temperature and humidity
using the **myDHT beginner layer**.

The goal of this project is simple:

> **Plug the sensor, upload the sketch, and get reliable readings — no configuration required.**

This demo is intentionally minimal and serves as the **entry point** for new myDHT users.

---

## What This Demo Demonstrates

✔ Zero configuration  
✔ Automatic sensor detection (DHT11 / DHT22)  
✔ Safe read intervals enforced automatically  
✔ Stable, cached readings  
✔ Clean and beginner-friendly API  

No defines.  
No timing calculations.  
No random freezes.

---

## Supported Hardware

- **Boards:** Arduino Uno, Arduino Nano (including compatible clones)
- **Sensors:** DHT11, DHT22
- **Connection:** Single digital pin

---

## Wiring

| DHT Sensor Pin | Arduino |
|---------------|---------|
| VCC           | 5V      |
| DATA          | D2      |
| GND           | GND     |

> Most DHT modules already include the required pull-up resistor.  
> If using a bare sensor, add a 4.7k–10kΩ pull-up between VCC and DATA.

---

## Code Overview

This demo uses the **myDHT beginner layer**, designed for first-time users.

```cpp
#include <myDHT.h>

const int DHT_PIN = 2;
myDHT dht(DHT_PIN);  // Auto-detects DHT11 or DHT22
```

That is all the setup required.

The library:

- Detects the sensor type automatically
- Enforces the correct minimum read interval
- Caches values internally for safety

---

## Output

Example Serial Monitor output:

```
Temperature: 23.4 °C
Humidity:    45 %
Dew Point:   9.2 °C
Heat Index:  24.0 °C
```

Values update at a sensor-safe interval, managed internally by the library.

---

## Why Use myDHT Instead of Other DHT Libraries?

Most DHT libraries require users to:

- Manually define the sensor type
- Manually handle timing restrictions
- Add large delays that freeze the loop
- Debug random NaN or checksum errors

**myDHT eliminates these problems by design.**

---

### Key Advantages Shown in This Demo

- Auto-detection: No configuration errors
- Safety: You cannot read the sensor too fast
- Stability: Cached values prevent invalid reads
- Simplicity: Clean, readable code

This demo alone replaces dozens of “why does my DHT not work?” forum threads.

---

## Who Is This Demo For?

✔ Absolute beginners  
✔ Students and educators  
✔ Quick prototypes  
✔ Anyone who just wants correct values without hassle

If you want advanced control, async reads, or multi-sensor setups,
see the next demos using the myDHTPro layer.

---

## Next Steps

After this demo, continue with upcoming beginner projects:

- **Demo 02 – Dew Point Greenhouse Indicator**  
  Reads temperature, humidity, and dew point.  
  LED indicator shows whether conditions are too dry, too humid, or optimal for plants.

- **Demo 03 – Heat Index Gauge**  
  Uses heat index to control a servo motor.  
  The servo points to a scale showing the level of heat danger, similar to fire-risk indicators in parks and forests.

These upcoming demos will continue to build on the beginner layer, requiring minimal rewiring and no advanced configuration.

---

## About myDHT

myDHT is a layered, zero-dependency DHT library for Arduino:

- Beginner layer (myDHT) — simple, safe, reliable
- Advanced layer (myDHTPro) — full protocol access, async reads, debug tools

Start simple.  
Scale when you need more control.

---