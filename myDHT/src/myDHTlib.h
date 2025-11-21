#ifndef MYDHTLIB_H
#define MYDHTLIB_H

#include <Arduino.h>

class MyDHT
{
public:
    MyDHT(uint8_t pin);
    void begin();
    float getTemperature();
    float getHumidity();
    bool read();

private:
    uint8_t _pin;
    int readOneBit();
    uint8_t readByte();
    uint8_t _byte1, _byte2, _byte3, _byte4, _byte5;
};

#endif