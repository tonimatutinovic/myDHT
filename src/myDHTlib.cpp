/*
  MyDHT Library
  Supports DHT11 and DHT22
  Provides temperature (C/F/K), humidity, dew point, heat index
  Author: Your Name
  Version: 1.0
*/

#include "myDHTlib.h"
#include <math.h>

/*
  Constructor
  @param pin Arduino pin
  @param type Sensor type (DHT11 or DHT22)
  @param retries Number of retry attempts if read fails
*/
MyDHT::MyDHT(uint8_t pin, DHTType type, uint8_t retries)
{
    _pin = pin;
    _type = type;
    _retries = retries;
}

// Initialize the sensor (set pin mode)
void MyDHT::begin()
{
    pinMode(_pin, INPUT_PULLUP);
}

/*
  Read sensor data with retry mechanism
  @return DHTError code
*/
DHTError MyDHT::read()
{
    DHTError err;

    for (uint8_t attempt = 0; attempt < _retries; attempt++)
    {
        err = readOnce();
        if (err == DHT_OK)
            return DHT_OK;
        delay(50); // Short delay before retry
    }

    return err;
}

/*
  Perform a single read attempt from the sensor
  @return DHTError code
*/
DHTError MyDHT::readOnce()
{
    // Send start signal
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    delay(20); // Pull LOW for 20ms
    digitalWrite(_pin, HIGH);
    delayMicroseconds(30); // Then pull HIGH for 30µs
    pinMode(_pin, INPUT_PULLUP);

    // Wait for sensor ACK (empirical timeouts)
    unsigned long timer = micros();
    while (digitalRead(_pin) == HIGH)
    {
        if (micros() - timer > 1000) // Timeout
        {
            return DHT_NO_RESPONSE;
        }
    }

    timer = micros();
    while (digitalRead(_pin) == LOW)
    {
        if (micros() - timer > 1000)
        {
            return DHT_ACK_TIMEOUT;
        }
    }

    timer = micros();
    while (digitalRead(_pin) == HIGH)
    {
        if (micros() - timer > 1000)
        {
            return DHT_ACK_TIMEOUT;
        }
    }

    // Read 5 bytes
    _byte1 = readByte();
    if (_byte1 == 0xFF)
        return DHT_BIT_TIMEOUT;
    _byte2 = readByte();
    if (_byte2 == 0xFF)
        return DHT_BIT_TIMEOUT;
    _byte3 = readByte();
    if (_byte3 == 0xFF)
        return DHT_BIT_TIMEOUT;
    _byte4 = readByte();
    if (_byte4 == 0xFF)
        return DHT_BIT_TIMEOUT;
    _byte5 = readByte();
    if (_byte5 == 0xFF)
        return DHT_BIT_TIMEOUT;

    // Verify checksum
    uint8_t sum = _byte1 + _byte2 + _byte3 + _byte4;
    if (sum != _byte5)
    {
        return DHT_CHECKSUM_FAIL;
    }

    return DHT_OK;
}

/*
  Get last read humidity
  @return Relative humidity (%)
*/
float MyDHT::getHumidity()
{
    float hum;
    if (_type == DHT11)
    {
        hum = _byte1 + _byte2 / 10.0;
    }
    else // DHT22
    {
        uint16_t raw = (_byte1 << 8) | _byte2;
        hum = raw * 0.1;
    }

    hum += _humidityOffset;

    // Clamp to 0–100%
    if (hum < 0.0)
        hum = 0.0;
    if (hum > 100.0)
        hum = 100.0;

    return hum;
}

/*
  Get last read temperature in Celsius
  @return Temperature in °C
*/
float MyDHT::getTemperature(TempUnit unit)
{
    float tempC;

    if (_type == DHT11)
    {
        tempC = _byte3 + _byte4 / 10.0;
    }
    else // DHT22
    {
        uint16_t raw = ((_byte3 & 0x7F) << 8) | _byte4;
        tempC = raw * 0.1;

        // Check for negative temperature
        if (_byte3 & 0x80)
        {
            tempC = -tempC;
        }
    }

    tempC += _tempOffsetC; // apply calibration offset

    switch (unit)
    {
    case Celsius:
        return tempC;
    case Fahrenheit:
        return tempC * 9.0 / 5.0 + 32;
    case Kelvin:
        return tempC + 273.15;
    default:
        return tempC;
    }

    return tempC;
}

/*
  Calculate dew point based on last reading
  Uses Magnus formula
  @return Dew point in current temperature unit
*/
float MyDHT::getDewPoint(TempUnit unit)
{
    float T = getTemperature(Celsius);
    float RH = getHumidity();
    double a = 17.27;
    double b = 237.7;
    double alpha = ((a * T) / (b + T)) + log(RH / 100.0);
    double dewPointC = (b * alpha) / (a - alpha);

    // Convert to selected unit
    switch (unit)
    {
    case Celsius:
        return dewPointC;
    case Fahrenheit:
        return dewPointC * 9.0 / 5.0 + 32;
    case Kelvin:
        return dewPointC + 273.15;
    default:
        return dewPointC;
    }
}

/*
  Calculate heat index ("feels like" temperature)
  Uses Rothfusz regression formula
  @return Heat Index in current temperature unit
*/
float MyDHT::getHeatIndex(TempUnit unit)
{
    float T = getTemperature(Fahrenheit); // HI formula in F
    float RH = getHumidity();

    // Simple formula for HI < 80°F
    float HI = 0.5 * (T + 61.0 + ((T - 68.0) * 1.2) + (RH * 0.094));

    // Apply full Rothfusz regression if HI >= 80°F
    if (HI >= 80.0)
    {
        HI = -42.379 + 2.04901523 * T + 10.14333127 * RH - 0.22475541 * T * RH - 0.00683783 * T * T - 0.05481717 * RH * RH + 0.00122874 * T * T * RH + 0.00085282 * T * RH * RH - 0.00000199 * T * T * RH * RH;
    }

    // Convert to user-selected unit
    switch (unit)
    {
    case Celsius:
        return (HI - 32) * 5.0 / 9.0;
    case Fahrenheit:
        return HI;
    case Kelvin:
        return (HI - 32) * 5.0 / 9.0 + 273.15;
    default:
        return (HI - 32) * 5.0 / 9.0;
    }
}

/*
  Set temperature calibration offset
*/
void MyDHT::setTemperatureOffset(float offsetC)
{
    _tempOffsetC = offsetC;
}

/*
  Set humidity calibration offset
*/
void MyDHT::setHumidityOffset(float offset)
{
    _humidityOffset = offset;
}

/*
 Set number of retry attempts for read()
*/
void MyDHT::setRetries(uint8_t retries)
{
    _retries = retries;
}

/*
  Read a single bit from the sensor
  @return 0, 1, or -1 on timeout/error
*/
int MyDHT::readOneBit()
{
    unsigned long t = micros();
    // Wait for LOW signal
    while (digitalRead(_pin) == HIGH)
    {
        if (micros() - t > 120) // 120µs timeout
            return -1;
    }

    t = micros();
    // Measure LOW duration
    while (digitalRead(_pin) == LOW)
    {
        if (micros() - t > 100) // 100µs timeout
            return -1;
    }

    unsigned long highStart = micros();
    // Start HIGH timer
    while (digitalRead(_pin) == HIGH)
    {
        if (micros() - highStart > 120) // 120µs timeout
            return -1;
    }

    unsigned long highDuration = micros() - highStart;
    // Determine bit value based on HIGH pulse duration
    if (highDuration > 40)
        return 1;
    else
        return 0;
}

/*
  Read a byte (8 bits) from the sensor
  @return Byte value or 0xFF on error
*/
uint8_t MyDHT::readByte()
{
    uint8_t result = 0;

    for (int i = 0; i < 8; i++)
    {
        int bit = readOneBit();

        if (bit < 0)
            return 0xFF;

        result <<= 1;
        result |= bit;
    }

    return result;
}