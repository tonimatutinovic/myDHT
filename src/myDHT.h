#ifndef MYDHT_H
#define MYDHT_H

#include "myDHTPro.h"
#include <Arduino.h>

class myDHT
{
public:
    // Constructor: pin is mandatory, type optional (autodetect)
    myDHT(uint8_t pin, DHTType type = DHT_AUTO);

    // Initialize the sensor (call in setup)
    void begin();

    // Beginner-friendly API
    float getTemperature(TempUnit unit = Celsius); // Returns temperature
    float getHumidity();                           // Returns relative humidity
    float dewPoint(TempUnit unit = Celsius);       // Returns dew point
    float HiIndex(TempUnit unit = Celsius);        // Returns heat index

    // Utilities
    DHTType getType();                                 // Returns sensor type
    void setOffset(float tempOffset, float humOffset); // Set calibration offsets
    void enableFriendlyErrors(bool enable = true);     // Enable/disable user-friendly errors
    unsigned long getMinInterval();                    // Minimum recommended read interval

private:
    MyDHT _sensor;               // Underlying advanced sensor object
    bool _friendlyErrors = true; // Flag for friendly errors

    // Cached data
    unsigned long _lastReadTime = 0;
    float _lastTemp = NAN;
    float _lastHum = NAN;
    bool _hasValidData = false;

    unsigned long _minInterval(); // Internal min interval
    bool _readSensor();           // Read sensor with caching
};

#endif