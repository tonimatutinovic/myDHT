/*
  Example: MyDHT Memory Optimized Build Demo
  Shows how optimized build works without debug/test modes
*/

#include <myDHTlib.h>

// CONFIG: set DHT_OPTIMIZED_BUILD true/false in myDHT_config.h

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

#if !DHT_OPTIMIZED_BUILD
    // Only in full build: enable debug/test modes
    dht.debugMode = true;
    dht.testMode = true;
    Serial.println("Full-featured build: debug and test modes enabled");
#else
    Serial.println("Optimized build: debug and test modes disabled");
#endif
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

    delay(2000); // Wait 2 sec before next read
}