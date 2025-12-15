#include <myDHTPro.h>

// Pin where the sensor is connected
const int DHT_PIN = 2;

// ---------------------
// Initialize sensor
// ---------------------

// For DHT11
MyDHT dht(DHT_PIN);

// For DHT22, uncomment the line below and comment out the DHT11 line
// MyDHT dht(DHT_PIN, DHT22)

// Callback function called when async read finishes
void dhtCallback(DHTData data)
{
    if (data.status == DHT_OK)
    {
        // If reading was successful, print temperature, humidity, dew point, and heat index
        Serial.print("Temp: ");
        Serial.print(data.temp);
        Serial.print(" °C, Hum: ");
        Serial.print(data.hum);
        Serial.print(" %, Dew: ");
        Serial.print(data.dew);
        Serial.print(" °C, HI: ");
        Serial.println(data.hi);
    }
    else
    {
        // Print error code if reading failed
        Serial.print("DHT Error: ");
        Serial.println(data.status);
    }
}

void setup()
{
    Serial.begin(115200); // Initialize serial communication
    dht.begin();          // Initialize DHT sensor
    delay(2000);          // Wait for sensor to stabilize
}

void loop()
{
    static unsigned long lastRead = 0;

    // DHT11 limitation: max 1Hz
    if ((millis() - lastRead >= 1000) && !dht.isReading())
    {
        lastRead = millis();
        dht.startAsyncRead(dhtCallback);
    }

    // DHT22 can be read as fast as possible, no delay necessary
    // if (!dht.isReading()) dht.startAsyncRead(dhtCallback);

    // Always call processAsync() to run async state machine
    dht.processAsync();

    // Other tasks can run here
    // e.g., LED blink, sensors, etc.
}