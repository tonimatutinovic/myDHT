#include <myDHT.h>

#define DHTPIN 2
myDHT dht(DHTPIN);

void setup()
{
    Serial.begin(115200);
    dht.begin();
}

void loop()
{
    Serial.print("Celsius: ");
    Serial.print(dht.getTemperature(Celsius)); // Celsius is default, getTemperature() works the same
    Serial.print(" °C, Fahrenheit: ");
    Serial.print(dht.getTemperature(Fahrenheit)); // Read temperature in Fahrenheit
    Serial.print(" °F, Kelvin: ");
    Serial.println(dht.getTemperature(Kelvin)); // Read temperature in Fahrenheit

    delay(dht.getMinInterval());
}