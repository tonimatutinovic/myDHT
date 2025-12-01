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
    DHT_OK = 0,
    DHT_NO_RESPONSE,
    DHT_ACK_TIMEOUT,
    DHT_CHECKSUM_FAIL,
    DHT_BIT_TIMEOUT
};

class MyDHT
{
public:
    /*
     * Constructor
     * @param pin   Arduino pin number
     * @param type  Sensor type (DHT11 or DHT22)
     * @param retries Number of retry attempts if read fails (default 3)
     */
    MyDHT(uint8_t pin, DHTType type, uint8_t retries = 3);

    // Initialize the sensor (set pin mode, etc.)
    void begin();

    // Returns last read temperature in Celsius
    float getTemperature();

    // Returns last read humidity in %
    float getHumidity();

    // Read new data from sensor, returns DHTError
    DHTError read();

private:
    uint8_t _pin;     // Pin where sensor is connected
    DHTType _type;    // Sensor type
    uint8_t _retries; // Number of retries

    // Low-level read functions
    int readOneBit();    // Reads a single bit from the sensor
    uint8_t readByte();  // Reads a byte (8 bits) from the sensor
    DHTError readOnce(); // Performs a single read attempt
    // Last read bytes from the sensor
    uint8_t _byte1, _byte2, _byte3, _byte4, _byte5;
};

#endif