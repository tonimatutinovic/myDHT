#include <myDHTlib.h>

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
        // Raw sensor bytes
        DHTRawData raw = dht.getRawData();

        Serial.println("Raw sensor bytes:");
        Serial.print("Byte1: ");
        Serial.println(raw.byte1);
        Serial.print("Byte2: ");
        Serial.println(raw.byte2);
        Serial.print("Byte3: ");
        Serial.println(raw.byte3);
        Serial.print("Byte4: ");
        Serial.println(raw.byte4);
        Serial.print("Byte5: ");
        Serial.println(raw.byte5);

        Serial.println("----------------");
    }
    else
    {
        Serial.print("Error reading sensor: ");
        Serial.println(err);
    }

    delay(2000);
}