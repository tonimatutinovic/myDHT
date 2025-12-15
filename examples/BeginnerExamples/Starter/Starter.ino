#include <myDHT.h>

#define DHTPIN 2
// Initialize myDHT on pin 2, auto-detect sensor type
myDHT dht(DHTPIN);

void setup()
{
    Serial.begin(115200);
    dht.begin(); // Initialize sensor
}

void loop()
{
    Serial.print("Temperature: ");
    Serial.print(dht.getTemperature()); // Read temperature in Celsius
    Serial.print(" °C, Humidity: ");
    Serial.println(dht.getHumidity()); // Read humidity

    delay(dht.getMinInterval()); // Wait minimum interval before next read
}