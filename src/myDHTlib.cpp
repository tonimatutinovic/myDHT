#include "myDHTlib.h"

/*
 * Constructor
 * @param pin Arduino pin
 * @param type Sensor type (DHT11 or DHT22)
 * @param retries Number of retry attempts if read fails
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
 * Perform a single read attempt from the sensor
 * @return DHTError code
 */
DHTError MyDHT::readOnce()
{
    // Send start signal
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    delay(20); // Pull low for 20ms
    digitalWrite(_pin, HIGH);
    delayMicroseconds(30); // Then pull high for 30µs
    pinMode(_pin, INPUT_PULLUP);

    // Wait for sensor ACK
    unsigned long timer = micros();
    while (digitalRead(_pin) == HIGH)
    {
        if (micros() - timer > 1000)
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
 * Read sensor data with retry mechanism
 * @return DHTError code
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
 * Get last read humidity
 * @return Relative humidity (%)
 */
float MyDHT::getHumidity()
{
    if (_type == DHT11)
    {
        return _byte1 + _byte2 / 10.0;
    }
    else // DHT22
    {
        uint16_t raw = (_byte1 << 8) | _byte2;
        return raw * 0.1;
    }
}

/*
 * Get last read temperature
 * @return Temperature in Celsius
 */
float MyDHT::getTemperature()
{
    if (_type == DHT11)
    {
        return _byte3 + _byte4 / 10.0;
    }
    else // DHT22
    {
        uint16_t raw = ((_byte3 & 0x7F) << 8) | _byte4;
        float temp = raw * 0.1;

        // Check for negative temperature
        if (_byte3 & 0x80)
        {
            temp = -temp;
        }

        return temp;
    }
}

/*
 * Read a single bit from the sensor
 * @return 0, 1, or -1 on timeout/error
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
 * Read a byte (8 bits) from the sensor
 * @return Byte value or 0xFF on error
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