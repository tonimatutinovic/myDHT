#ifndef MYDHTLIB_H
#define MYDHTLIB_H

#include <Arduino.h>

enum DHTType
{
    DHT11,
    DHT22
};

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
    MyDHT(uint8_t pin, DHTType type, uint8_t retries = 3);
    void begin();
    float getTemperature();
    float getHumidity();
    DHTError read();

private:
    uint8_t _pin;
    DHTType _type;
    uint8_t _retries;
    int readOneBit();
    uint8_t readByte();
    DHTError readOnce();
    uint8_t _byte1, _byte2, _byte3, _byte4, _byte5;
};

#endif