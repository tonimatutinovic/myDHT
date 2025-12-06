#include <myDHTlib.h>

// Pin where the sensor is connected
const int DHT_PIN = 2;

// ---------------------
// Initialize sensor
// ---------------------

MyDHT dht(DHT_PIN); // Auto detect type

void setup()
{
    Serial.begin(115200);
    //delay(2000);

    // --- AUTO DETECT ---
    dht.begin();

    // --- GET TYPE ---
    Serial.print("Auto-detected type: ");
    switch (dht.getType())
    {
    case DHT11:
        Serial.println("DHT11");
        break;
    case DHT22:
        Serial.println("DHT22");
        break;
    default:
        Serial.println("UNKNOWN");
        break;
    }

    // --- GET MIN INTERVAL ---
    Serial.print("Min read interval (ms): ");
    Serial.println(dht.getMinReadInterval());

    // --- TEST SETTER ---
    Serial.println("Setting type manually to DHT11...");
    dht.setType(DHT11);

    Serial.print("Type after manual set: ");
    switch (dht.getType())
    {
    case DHT11:
        Serial.println("DHT11");
        break;
    case DHT22:
        Serial.println("DHT22");
        break;
    default:
        Serial.println("UNKNOWN");
        break;
    }
}

void loop()
{
}