#include <myDHTPro.h>

// Pin where the sensor is connected
const uint8_t DHT_PIN = 2;

// Create MyDHT object with auto-detect
MyDHT dht(DHT_PIN);

void setup()
{
    Serial.begin(115200);

    // --- Initialize sensor and auto-detect type ---
    dht.begin();

    // --- Display detected type ---
    Serial.print("Sensor type detected: ");
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
    // Perform a read and get error status
    DHTError err = dht.read();

    // Display temperature and humidity only if read successful
    if (err == DHT_OK)
    {
        Serial.print("Temperature (C): ");
        Serial.println(dht.getTemperature(Celsius));
        Serial.print("Humidity (%): ");
        Serial.println(dht.getHumidity());
    }
    else
    {
        Serial.print("Read error: ");
        Serial.println(dht.getErrorString(err));
    }

    // Display error information
    Serial.print("Last error: ");
    Serial.println(dht.getErrorString(dht.getLastError()));
    Serial.print("Consecutive failures: ");
    Serial.println(dht.getFailureCount());
    Serial.print("Sensor connected? ");
    Serial.println(dht.isConnected() ? "Yes" : "No");

    Serial.println("-------------------------");

    delay(dht.getMinReadInterval());
}
