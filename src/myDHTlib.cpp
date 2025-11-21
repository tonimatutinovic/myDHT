#include "myDHTlib.h"

MyDHT::MyDHT(uint8_t pin)
{
    _pin = pin;
}

void MyDHT::begin()
{
    pinMode(_pin, INPUT_PULLUP);
}

bool MyDHT::read()
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
            Serial.print("Senzor ne reagira!");
            return false;
        }
    }

    timer = micros();
    while (digitalRead(_pin) == LOW)
    {
        if (micros() - timer > 1000)
        {
            Serial.print("Timeout za ACK!");
            return false;
        }
    }

    timer = micros();
    while (digitalRead(_pin) == HIGH)
    {
        if (micros() - timer > 1000)
        {
            Serial.print("Timeout za ACK!");
            return false;
        }
    }

    // Čitanje 5 bajtova
    _byte1 = readByte();
    _byte2 = readByte();
    _byte3 = readByte();
    _byte4 = readByte();
    _byte5 = readByte();

    // Provjera checksum-a
    uint8_t sum = _byte1 + _byte2 + _byte3 + _byte4;
    if (sum != _byte5)
    {
        Serial.print("Checksum neodgovara!");
        return false;
    }

    return true;
}

float MyDHT::getHumidity()
{
    return _byte1 + _byte2 / 10.0;
}

float MyDHT::getTemperature()
{
    return _byte3 + _byte4 / 10.0;
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