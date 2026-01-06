#include "myDHT.h"
#include <math.h>

// Constructor: initialize underlying sensor
myDHT::myDHT(uint8_t pin, DHTType type)
    : _sensor(pin, type)
{
}

// Initialize sensor
void myDHT::begin()
{
    _sensor.begin();
}

// Minimal interval
unsigned long myDHT::_minInterval()
{
    // Reliability-first for both DHT11 and DHT22
    return 2000;
}

// Internal sensor read with caching
bool myDHT::_readSensor()
{
    unsigned long now = millis();

    // Return cached values if interval not passed
    if (_hasValidData && (now - _lastReadTime < _minInterval()))
        return true;

    // Read sensor
    DHTError err = _sensor.read();
    _lastReadTime = now;

    if (err != DHT_OK)
    {
        _hasValidData = false;
        if (_friendlyErrors)
        {
            Serial.print("DHT friendly error: ");
            Serial.println(_sensor.getErrorString(err));
        }
        return false;
    }

    // Cache values
    _lastTemp = _sensor.getTemperature(Celsius);
    _lastHum = _sensor.getHumidity();
    _hasValidData = true;
    return true;
}

// Get temperature in specified unit
float myDHT::getTemperature(TempUnit unit)
{
    if (!_readSensor())
        return NAN;

    float t = _lastTemp;
    switch (unit)
    {
    case Celsius:
        return t;
    case Fahrenheit:
        return t * 9.0 / 5.0 + 32;
    case Kelvin:
        return t + 273.15;
    default:
        return t;
    }
}

// Get relative humidity
float myDHT::getHumidity()
{
    if (!_readSensor())
        return NAN;

    return _lastHum;
}

// Calculate dew point
float myDHT::dewPoint(TempUnit unit)
{
    if (!_readSensor())
        return NAN;

    double T = _lastTemp;
    double RH = _lastHum;
    double a = 17.27;
    double b = 237.7;
    double alpha = ((a * T) / (b + T)) + log(RH / 100.0);
    double dewC = (b * alpha) / (a - alpha);

    switch (unit)
    {
    case Celsius:
        return dewC;
    case Fahrenheit:
        return dewC * 9.0 / 5.0 + 32;
    case Kelvin:
        return dewC + 273.15;
    default:
        return dewC;
    }
}

// Calculate heat index
float myDHT::HiIndex(TempUnit unit)
{
    if (!_readSensor())
        return NAN;

    float T = _lastTemp * 9.0 / 5.0 + 32; // Fahrenheit
    float RH = _lastHum;

    float HI = 0.5 * (T + 61.0 + ((T - 68.0) * 1.2) + (RH * 0.094));
    if (HI >= 80.0)
    {
        // Rothfusz regression
        HI = -42.379 + 2.04901523 * T + 10.14333127 * RH - 0.22475541 * T * RH - 0.00683783 * T * T - 0.05481717 * RH * RH + 0.00122874 * T * T * RH + 0.00085282 * T * RH * RH - 0.00000199 * T * T * RH * RH;
    }

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

// Return sensor type
DHTType myDHT::getType()
{
    return _sensor.getType();
}

// Set calibration offsets
void myDHT::setOffset(float tempOffset, float humOffset)
{
    _sensor.setTemperatureOffset(tempOffset);
    _sensor.setHumidityOffset(humOffset);
}

// Enable or disable friendly errors
void myDHT::enableFriendlyErrors(bool enable)
{
    _friendlyErrors = enable;
}

// Minimum interval between reads
unsigned long myDHT::getMinInterval()
{
    return _minInterval();
}