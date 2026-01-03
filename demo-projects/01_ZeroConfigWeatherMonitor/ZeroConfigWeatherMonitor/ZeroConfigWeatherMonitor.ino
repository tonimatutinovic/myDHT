/*
  Demo 01 – Zero-Config Weather Monitor
  ------------------------------------
  Demonstrates the myDHT beginner layer:
  - Zero configuration
  - Automatic sensor detection (DHT11 / DHT22)
  - Enforced safe read interval
  - Reliable, cached readings

  Hardware:
  - Arduino Uno / Nano
  - DHT11 or DHT22 on pin 2
*/

#include <myDHT.h> // Beginner layer

const int DHT_PIN = 2;
myDHT dht(DHT_PIN); // Auto-detect sensor type

void setup()
{
    Serial.begin(115200);
    dht.begin(); // Initialize sensor
}

void loop()
{
    // You can use Celsius, Fahrenheit, or Kelvin here (Celsius is default)
    float temperature = dht.getTemperature(Celsius);
    float humidity = dht.getHumidity();
    float dewPoint = dht.dewPoint(Celsius);
    float heatIndex = dht.HiIndex(Celsius);

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humidity:    ");
    Serial.print(humidity);
    Serial.println(" %");

    Serial.print("Dew Point:   ");
    Serial.print(dewPoint);
    Serial.println(" °C");

    Serial.print("Heat Index:  ");
    Serial.print(heatIndex);
    Serial.println(" °C");

    Serial.println("----------------------");

    // Enforced sensor-safe interval
    delay(dht.getMinInterval());
}