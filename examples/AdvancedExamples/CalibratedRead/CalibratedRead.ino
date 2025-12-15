#include <myDHTPro.h>

const int DHT_PIN = 2;

// For DHT11
MyDHT dht(DHT_PIN, DHT11);
// For DHT22, uncomment the line below
// MyDHT dht(DHT_PIN, DHT22)

void setup()
{
    Serial.begin(115200);
    dht.begin();

    delay(2000);

    // Optional: set calibration offsets
    dht.setTemperatureOffset(1.5); // +1.5°C
    dht.setHumidityOffset(-2.0);   // -2% RH
}

void loop()
{
    DHTError err = dht.read();

    if (err == DHT_OK)
    {
        Serial.print("Temperature (C): ");
        Serial.println(dht.getTemperature(Celsius));

        Serial.print("Temperature (F): ");
        Serial.println(dht.getTemperature(Fahrenheit));

        Serial.print("Temperature (K): ");
        Serial.println(dht.getTemperature(Kelvin));

        Serial.print("Humidity (%): ");
        Serial.println(dht.getHumidity());

        Serial.print("Dew Point: ");
        Serial.println(dht.getDewPoint(Celsius));

        Serial.print("Heat Index: ");
        Serial.println(dht.getHeatIndex(Fahrenheit));

        Serial.println("----------------");
    }
    else
    {
        Serial.print("Error reading sensor: ");
        Serial.println(err);
    }

    delay(2000);
}