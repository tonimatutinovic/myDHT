/*
  Demo 02 – Dew-Point Greenhouse Indicator
  ---------------------------------------
  Uses dew point to estimate condensation risk in a greenhouse.
  RGB LED shows status:
    - Green  : Safe (T - DP >= 5°C)
    - Yellow : Watch (3°C <= T - DP < 5°C)
    - Red    : Condensation risk (T - DP < 3°C)

  Hardware:
  - Arduino Uno / Nano
  - DHT11 or DHT22 on pin 2
  - RGB LED on pins 9,10,11 (with resistors)
*/

#include <myDHT.h>

const int DHT_PIN = 2;
myDHT dht(DHT_PIN); // Auto-detect

// PWM pins
const int PIN_R = 11;
const int PIN_G = 10;
const int PIN_B = 9;

void setRGB(uint8_t r, uint8_t g, uint8_t b)
{
  analogWrite(PIN_R, r);
  analogWrite(PIN_G, g);
  analogWrite(PIN_B, b);
}

void setup()
{
  Serial.begin(115200);
  dht.begin();

  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);

  setRGB(0, 0, 0);
}

void loop()
{
  float t = dht.getTemperature(Celsius);
  float h = dht.getHumidity();
  float dp = dht.dewPoint(Celsius);

  // Condensation proximity metric
  float spread = t - dp;

  // LED logic
  if (spread < 3.0) {
    // Red: high condensation risk
    setRGB(255, 0, 0);
  } else if (spread < 5.0) {
    // Yellow: caution
    setRGB(255, 120, 0);
  } else {
    // Green: safe
    setRGB(0, 255, 0);
  }

  // Show telemetry
  Serial.print("T: "); Serial.print(t); Serial.print(" C, ");
  Serial.print("RH: "); Serial.print(h); Serial.print(" %, ");
  Serial.print("DP: "); Serial.print(dp); Serial.print(" C, ");
  Serial.print("T-DP: "); Serial.print(spread); Serial.println(" C");
  Serial.println("----------------------------------------------------");

  delay(dht.getMinInterval());
}
