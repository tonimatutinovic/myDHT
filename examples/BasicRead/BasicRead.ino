#include <myDHTlib.h>

// Pin where the sensor is connected
const int DHT_PIN = 2;

// ---------------------
// Initialize sensor
// ---------------------

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
    // Temperature in all units
    float tempC = dht.getTemperature(Celsius);
    float tempF = dht.getTemperature(Fahrenheit);
    float tempK = dht.getTemperature(Kelvin);

    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.print(" °C, ");
    Serial.print(tempF);
    Serial.print(" °F, ");
    Serial.print(tempK);
    Serial.println(" K");

    // Humidity
    float humidity = dht.getHumidity();
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Dew point
    float dewPoint = dht.getDewPoint();
    Serial.print("Dew Point: ");
    Serial.println(dewPoint);

    // Heat Index
    float hi = dht.getHeatIndex();
    Serial.print("Heat Index: ");
    Serial.println(hi);

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