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
    unsigned long minInterval = dht.getMinInterval(); // Get minimum read interval for sensor detected
    Serial.print("Minimum read interval: ");
    Serial.print(minInterval);
    Serial.println(" ms");

    delay(minInterval);
}