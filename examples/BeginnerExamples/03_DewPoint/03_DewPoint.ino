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
    Serial.print("Dew Point: ");
    Serial.println(dht.dewPoint()); // Calculate dew point
    delay(dht.getMinInterval());
}