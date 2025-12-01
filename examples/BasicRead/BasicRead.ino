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
  DHTError err = dht.read();

  if (err == DHT_OK)
  {
    Serial.print("Temperature: ");
    Serial.println(dht.getTemperature());
    Serial.print("Humidity: ");
    Serial.println(dht.getHumidity());
    Serial.println("----------------");
  }
  else if (err == DHT_NO_RESPONSE)
  {
    Serial.println("Sensor not responding!");
  }
  else if (err == DHT_ACK_TIMEOUT)
  {
    Serial.println("ACK timeout!");
  }
  else if (err == DHT_CHECKSUM_FAIL)
  {
    Serial.println("Checksum error!");
  }
  else if (err == DHT_BIT_TIMEOUT)
  {
    Serial.println("Bit read timeout!");
  }
  delay(2000);
}