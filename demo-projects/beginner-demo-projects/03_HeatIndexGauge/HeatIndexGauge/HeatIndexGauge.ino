/*
  Demo 03 – Heat Index Gauge

  Reads Heat Index using a DHT sensor and visualizes it
  with a servo-driven analog gauge.

  The myDHT beginner layer handles sensor detection,
  safe read timing, and cached readings automatically.
*/

#include <myDHT.h> // Beginner layer
#include <Servo.h>

const int DHT_PIN = 2;
const int SERVO_PIN = 9;

myDHT dht(DHT_PIN); // Auto-detect sensor type
Servo gaugeServo;

// Heat Index range (Celsius) mapped to the servo (0..180 degrees)
const float HI_MIN_C = 20.0; // below this -> 0°
const float HI_MAX_C = 55.0; // above this -> 180°

void setup()
{
  Serial.begin(115200);

  // myDHT beginner layer init (no extra config needed)
  dht.begin();

  // Servo init
  gaugeServo.attach(SERVO_PIN);

  // Center the gauge on boot
  gaugeServo.write(90);
  delay(400);

  // Optional quick servo check to confirm wiring before trusting sensor logic.
  /*
  gaugeServo.write(0);   delay(400);
  gaugeServo.write(90);  delay(400);
  gaugeServo.write(180); delay(400);
  gaugeServo.write(90);  delay(400);
  */

  Serial.println("Demo 03 – Heat Index Gauge");
  Serial.println("---------------------------");
}

void loop()
{
  // Read values (beginner layer: safe interval + caching handled for you)
  float t = dht.getTemperature(Celsius);
  float rh = dht.getHumidity();
  float hi = dht.HiIndex(Celsius);

  // If anything fails, keep last needle position and try again later
  if (isnan(t) || isnan(rh) || isnan(hi))
  {
    Serial.println("Read failed (NaN). Check wiring and try again...");
    delay(dht.getMinInterval());
    return;
  }

  // Map Heat Index to servo angle (0..180)
  int angle;

  if (hi <= HI_MIN_C)
  {
    angle = 0;
  }
  else if (hi >= HI_MAX_C)
  {
    angle = 180;
  }
  else
  {
    // Linear mapping between HI_MIN_C and HI_MAX_C
    angle = (int)((hi - HI_MIN_C) * 180.0 / (HI_MAX_C - HI_MIN_C));
  }

  // Invert servo direction to match physical gauge orientation
  gaugeServo.write(180 - angle);

  // Serial output (telemetry)
  Serial.print("T: ");
  Serial.print(t);
  Serial.print(" C, ");
  Serial.print("RH: ");
  Serial.print(rh);
  Serial.print(" %, ");
  Serial.print("Heat Index: ");
  Serial.print(hi);
  Serial.print(" C, ");
  Serial.print("Needle: ");
  Serial.print(angle);
  Serial.println(" deg");
  Serial.println("------------------------------------------------------------");

  // myDHT provides the sensor-safe minimum interval (DHT11 vs DHT22 differs)
  delay(dht.getMinInterval());
}