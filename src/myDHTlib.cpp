#include "myDHTlib.h"

MyDHT::MyDHT(uint8_t pin, DHTType type)
{
    _pin = pin;
    _type = type;
}

void MyDHT::begin()
{
    pinMode(_pin, INPUT_PULLUP);
}

DHTError MyDHT::read()
{
    // Start signal
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    delay(20);
    digitalWrite(_pin, HIGH);
    delayMicroseconds(30);
    pinMode(_pin, INPUT_PULLUP);

    // Cekanje ACK
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

    // Čitanje 5 bajtova
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

    // Provjera checksum-a
    uint8_t sum = _byte1 + _byte2 + _byte3 + _byte4;
    if (sum != _byte5)
    {
        return DHT_CHECKSUM_FAIL;
    }

    return true;
}

float MyDHT::getHumidity()
{
    if (_type == DHT11)
    {
        return _byte1 + _byte2 / 10.0;
    }
    else
    {
        uint16_t raw = (_byte1 << 8) | _byte2;
        return raw * 0.1;
    }
}

float MyDHT::getTemperature()
{
    if (_type == DHT11)
    {
        return _byte3 + _byte4 / 10.0;
    }
    else
    {
        uint16_t raw = ((_byte3 & 0x7F) << 8) | _byte4;
        float temp = raw * 0.1;

        if (_byte3 & 0x80)
        {
            temp = -temp;
        }

        return temp;
    }
}

int MyDHT::readOneBit()
{
    unsigned long t = micros();
    while (digitalRead(_pin) == HIGH)
    {
        if (micros() - t > 120)
            return -1;
    }

    t = micros();
    while (digitalRead(_pin) == LOW)
    {
        if (micros() - t > 100)
            return -1;
    }

    unsigned long highStart = micros();
    while (digitalRead(_pin) == HIGH)
    {
        if (micros() - highStart > 120)
            return -1;
    }

    unsigned long highDuration = micros() - highStart;
    if (highDuration > 40)
        return 1;
    else
        return 0;
}

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