#include <myDHT.h>

#define DHTPIN 2
myDHT dht(DHTPIN);

void setup()
{
    Serial.begin(115200);
    dht.begin();

    // Calibration offsets
    dht.setOffset(0.5, -2.0); // +0.5°C, -2% humidity
}

void loop()
{
    Serial.print("Temperature: ");
    Serial.print(dht.getTemperature());
    Serial.print(" °C, Humidity: ");
    Serial.println(dht.getHumidity());
    delay(dht.getMinInterval());
}