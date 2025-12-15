#include <myDHTPro.h>

#define DHT_PIN 2

// Create a DHT object for DHT11 sensor
MyDHT dht(DHT_PIN, DHT11);
// For DHT22, uncomment below
// MyDHT dht(DHT_PIN, DHT22)

void setup()
{
    Serial.begin(115200); // Initialize serial communication
    dht.begin();

    delay(2000); // Wait 2 seconds for sensor stabilization
}

void loop()
{
    // Read all sensor data using the structured API
    DHTData data = dht.getData(Celsius);

    // Print the status first
    Serial.print(F("Status: "));
    switch (data.status)
    {
    case DHT_OK:
        Serial.println(F("OK"));
        break;
    case DHT_ERROR_TIMEOUT:
        Serial.println(F("Timeout"));
        break;
    case DHT_ERROR_CHECKSUM:
        Serial.println(F("Checksum error"));
        break;
    default:
        Serial.println(F("Unknown error"));
        break;
    }

    // Print all readings regardless of status
    // F() macro stores the string in Flash instead of RAM, saving memory
    Serial.print(F("Temperature: "));
    Serial.print(data.temp); // Display raw float value (could be NaN on error)
    Serial.println(F(" °C"));

    Serial.print(F("Humidity: "));
    Serial.print(data.hum);
    Serial.println(F(" %"));

    Serial.print(F("Dew Point: "));
    Serial.print(data.dew);
    Serial.println(F(" °C"));

    Serial.print(F("Heat Index: "));
    Serial.print(data.hi);
    Serial.println(F(" °C"));

    delay(2000); // Wait 2 seconds before next read
}