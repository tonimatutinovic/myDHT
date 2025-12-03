#ifndef MYDHTLIB_H
#define MYDHTLIB_H

#include <Arduino.h>

// Sensor type
enum DHTType
{
  DHT11,
  DHT22
};

// Error codes returned by read()
enum DHTError
{
  DHT_OK = 0,        // Read successful
  DHT_NO_RESPONSE,   // Sensor did not respond to start signal
  DHT_ACK_TIMEOUT,   // Sensor ACK not received in time
  DHT_CHECKSUM_FAIL, // Checksum mismatch
  DHT_BIT_TIMEOUT    // Timeout while reading a bit
};

// Temperature units
enum TempUnit
{
  Celsius,
  Fahrenheit,
  Kelvin
};

// DHT raw data
struct DHTRawData
{
  uint8_t byte1;
  uint8_t byte2;
  uint8_t byte3;
  uint8_t byte4;
  uint8_t byte5;
};

class MyDHT
{
public:
  /*
    Constructor
    @param pin   Arduino pin number
    @param type  Sensor type (DHT11 or DHT22)
    @param retries Number of retry attempts if read fails (default 3)
  */
  MyDHT(uint8_t pin, DHTType type, uint8_t retries = 3);

  // Initialize the sensor (set pin mode, etc.)
  void begin();

  // Returns last read temperature in Celsius
  float getTemperature(TempUnit unit = Celsius);

  // Returns last relative humidity in %
  float getHumidity();

  /*
    Calculate dew point based on last read temperature and humidity
    Uses the _unit member to determine input temperature unit
    Returns dew point in the same unit as _unit
  */
  float getDewPoint(TempUnit unit = Celsius);

  /*
    Calculate heat index (feels-like temperature) based on last read temperature and humidity
    Internally converts temperature to Fahrenheit for formula, returns in same unit as _unit
  */
  float getHeatIndex(TempUnit unit = Celsius);

  // Read new data from sensor, returns DHTError
  DHTError read();

  // Set number of retry attempts if read fails
  void setRetries(uint8_t retries);

  // Adjust calibration offset
  void setTemperatureOffset(float offsetC);
  void setHumidityOffset(float offset);

  // Returns raw bytes read from the sensor
  DHTRawData getRawData();

private:
  uint8_t _pin;                // Pin where sensor is connected
  DHTType _type;               // Sensor type
  uint8_t _retries;            // Number of retries
  float _tempOffsetC = 0.0;    // Calibration offset
  float _humidityOffset = 0.0; // Humidity offset

  // Low-level read functions
  int readOneBit();    // Reads a single bit from the sensor
  uint8_t readByte();  // Reads a byte (8 bits) from the sensor
  DHTError readOnce(); // Performs a single read attempt

  // Last read bytes from the sensor
  uint8_t _byte1, _byte2, _byte3, _byte4, _byte5;
};

#endif