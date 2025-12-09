#include <myDHT.h>

const int DHT_PIN = 2;

// Use DHT11 here:
MyDHT dht(DHT_PIN, DHT11);

/*
  ---------- USING DHT22 INSTEAD ----------
  1) Replace the above line with:
       MyDHT dht(DHT_PIN, DHT22);
  2) Replace the valid test bytes in loop() with DHT22-formatted ones.
     Example DHT22 raw bytes for 25.0°C, 40.0% RH:
       dht.setRawBytes(0x01, 0x90, 0x00, 0xFA, 0x8B);
  -----------------------------------------
*/

void setup()
{
  Serial.begin(115200);
  dht.begin();
  dht.testMode = true;
  Serial.println("Sanity Check & isConnected Test");
}

void loop()
{
  Serial.println("\n--- Valid Read ---");

  // Valid DHT11 test bytes → T = 30°C, H = 50%
  // (For DHT22 format, see instructions above)
  dht.setRawBytes(0x1E, 0x00, 0x32, 0x00, 0x50);

  DHTError err = dht.read();

  if (err == DHT_OK)
  {
    Serial.print("Temperature: ");
    Serial.println(dht.getTemperature(Celsius));

    Serial.print("Humidity: ");
    Serial.println(dht.getHumidity());
  }
  else
  {
    Serial.print("Error: ");
    Serial.println(dht.getErrorString(err));
  }

  delay(2000);

  Serial.println("\n--- Invalid Reads ---");

  for (int i = 1; i <= 6; i++)
  {

    // Invalid test bytes for both DHT11 and DHT22
    dht.setRawBytes(0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

    err = dht.read();

    if (err == DHT_ERROR_SANITY)
    {
      Serial.print("Invalid reading ");
      Serial.println(i);
    }
    else if (err == DHT_OK)
    {
      Serial.print("Reading ");
      Serial.print(i);
      Serial.println(" returned OK but data values are unexpected");
    }
    else
    {
      Serial.print("Other error ");
      Serial.println(i);
    }

    // Check sensor connection state after each failed read
    if (!dht.isConnected())
    {
      Serial.println("Sensor appears disconnected after multiple failures!");
    }

    delay(1000);
  }

  Serial.println("\n--- Test complete. Restarting in 10s ---");
  delay(10000);
}
