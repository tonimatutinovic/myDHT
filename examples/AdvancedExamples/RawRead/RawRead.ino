#include <myDHTPro.h>

const int DHT_PIN = 2;

// For DHT11
MyDHT dht(DHT_PIN, DHT11);
// For DHT22, uncomment below
// MyDHT dht(DHT_PIN, DHT22)

void setup()
{
    Serial.begin(115200);
    dht.begin();
    delay(2000); // stabilize sensor
}

void loop()
{
    DHTError err = dht.read();

    if (err == DHT_OK)
    {
        DHTRawData raw = dht.getRawData();

        Serial.println("Raw bit timings:");
        for (int i = 0; i < 40; i++)
        {
            Serial.print("Bit ");
            Serial.print(i);
            Serial.print(": LOW=");
            Serial.print(raw.lowTimes[i]);
            Serial.print("us, HIGH=");
            Serial.print(raw.highTimes[i]);
            Serial.println("us");
        }

        Serial.println("Sensor bytes:");
        for (int i = 0; i < 5; i++)
        {
            Serial.print("Byte ");
            Serial.print(i);
            Serial.print(": 0x");
            Serial.println(raw.bytes[i], HEX);
        }

        Serial.println("----------------");
    }
    else
    {
        Serial.print("Error reading sensor: ");
        Serial.println(err);
    }

    delay(5000);
}