/*
  Example: MyDHT Memory Optimized Build Demo
  ----------------------------------
  Demonstrates the "optimized build" configuration which removes debug/test features
  to reduce memory usage.

  Configure build mode in: myDHT_config.h
    #define DHT_OPTIMIZED_BUILD 0  // full
    #define DHT_OPTIMIZED_BUILD 1  // optimized
*/

#include <myDHTPro.h>

// Pin where DHT sensor is connected
#define DHT_PIN 2

// Create sensor instance
MyDHT dht(DHT_PIN);

void setup()
{
    Serial.begin(115200);

    Serial.println("=== MyDHT Memory Optimized Build Demo ===");

    // Start sensor
    dht.begin();

    // In full build you may enable debug output (testMode is for injected raw bytes)
    if (!mydht::optimizedBuild())
    {
        dht.debugMode = true;
        Serial.println("Full build: debug enabled");
    }
    else
    {
        Serial.println("Optimized build: debug/test features disabled");
    }
}

void loop()
{
    // Read data
    DHTData data = dht.getData(Celsius);

    // Check status
    if (data.status == DHT_OK)
    {
        Serial.print("Temperature: ");
        Serial.print(data.temp);
        Serial.print(" °C, ");
        Serial.print("Humidity: ");
        Serial.print(data.hum);
        Serial.print(" %, ");
        Serial.print("Dew Point: ");
        Serial.print(data.dew);
        Serial.print(" °C, ");
        Serial.print("Heat Index: ");
        Serial.println(data.hi);
    }
    else
    {
        Serial.print("Error reading sensor: ");
        Serial.println(dht.getErrorString(data.status));
    }

    delay(dht.getMinReadInterval());
}