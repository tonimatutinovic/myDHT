#include <myDHTlib.h>

// Pin where the sensor is connected
const int DHT_PIN = 2;

// Initialize sensor

// For DHT11
MyDHT dht(DHT_PIN, DHT11);

// For DHT22, uncomment the line below and comment out the DHT11 line
// MyDHT dht(DHT_PIN, DHT22)

void setup()
{
  // Start serial communication
  Serial.begin(115200);
  // Initialize DHT sensor (sets pin mode)
  dht.begin();
  // Wait for sensor to stabilize
  delay(2000);
}

void loop()
{
  // Read data from sensor with retry mechanism
  DHTError err = dht.read();

  // Check result and print
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
    Serial.println("Error: Sensor not responding!");
  }
  else if (err == DHT_ACK_TIMEOUT)
  {
    Serial.println("Error: ACK timeout!");
  }
  else if (err == DHT_CHECKSUM_FAIL)
  {
    Serial.println("Error: Checksum mismatch!");
  }
  else if (err == DHT_BIT_TIMEOUT)
  {
    Serial.println("Error: Bit read timeout!");
  }
  else
  {
    Serial.println("Error: Unknown!");
  }

  // Wait 2 seconds before next read
  delay(2000);
}