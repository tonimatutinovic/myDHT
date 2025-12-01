#include <myDHTlib.h>

const int DHT_PIN = 2;

// For DHT11
MyDHT dht(DHT_PIN, DHT11);

// For DHT22, uncomment the line below and comment out the DHT11 line
// MyDHT dht(DHT_PIN, DHT22)

void setup()
{
  Serial.begin(115200);
  dht.begin();

  delay(2000);
}

void loop()
{
  if (dht.read())
  {
    Serial.print("Temperature: ");
    Serial.println(dht.getTemperature());
    Serial.print("Humidity: ");
    Serial.println(dht.getHumidity());
    Serial.println("----------------");
  }
  else
  {
    Serial.println("Failed to read from sensor!");
  }
  delay(2000);
}