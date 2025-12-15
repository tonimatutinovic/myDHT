#include <myDHT.h>

#define DHTPIN 2
myDHT dht(DHTPIN);

void setup()
{
    Serial.begin(115200);
    dht.begin();

    // Enable friendly error messages
    dht.enableFriendlyErrors(true);
}

void loop()
{
    float temp = dht.getTemperature();
    float hum = dht.getHumidity();

    if (isnan(temp) || isnan(hum))
    {
        Serial.println("Error reading sensor!");
    }
    else
    {
        Serial.print("Temperature: ");
        Serial.print(temp);
        Serial.print(" °C, Humidity: ");
        Serial.println(hum);
    }

    delay(dht.getMinInterval());
}