/*
  MyDHT Library
  Supports DHT11 and DHT22
  Provides temperature (C/F/K), humidity, dew point, heat index
  Author: Toni Matutinović
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

    // Set timing parameters depending on sensor type
    if (_type == DHT11)
    {
        _timings = {18, 5000, 120, 50};
    }
    else
    { // DHT22
        _timings = {1, 1000, 80, 40};
    }

    _state = IDLE;
    _hasLastValidData = false;
}

/*
  Read sensor data with retry mechanism
  @return DHTError code
*/
DHTError MyDHT::read()
{
    DHTError err;
    uint16_t retryDelay = (_type == DHT11) ? 50 : 20;

    for (uint8_t attempt = 0; attempt < _retries; attempt++)
    {
        err = readOnce();
        if (err == DHT_OK)
            return DHT_OK;
        delay(retryDelay); // Short delay before retry
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
    delay(_timings.startLowMs); // Pull LOW for 20ms
    digitalWrite(_pin, HIGH);
    delayMicroseconds(30); // Then pull HIGH for 30µs
    pinMode(_pin, INPUT_PULLUP);

    // Wait for sensor ACK (empirical timeouts)
    unsigned long timer = micros();
    while (digitalRead(_pin) == HIGH)
    {
        if (micros() - timer > _timings.ackTimeoutUs) // Timeout
        {
            return DHT_NO_RESPONSE;
        }
    }

    timer = micros();
    while (digitalRead(_pin) == LOW)
    {
        if (micros() - timer > _timings.ackTimeoutUs)
        {
            return DHT_ACK_TIMEOUT;
        }
    }

    timer = micros();
    while (digitalRead(_pin) == HIGH)
    {
        if (micros() - timer > _timings.ackTimeoutUs)
        {
            return DHT_ACK_TIMEOUT;
        }
    }

    return read5Bytes();
}

/*
  Reads 5 bytes of data from the DHT sensor
  Internally calls readByte() for each byte and verifies the checksum
  @return DHT_OK on success, otherwise one of the DHTError codes:
        DHT_BIT_TIMEOUT if a bit read timed out
        DHT_CHECKSUM_FAIL if checksum validation fails
*/
DHTError MyDHT::read5Bytes()
{
    int bitCounter = 0;

    // Read 5 bytes
    _byte1 = readByte(bitCounter);
    if (_byte1 == 0xFF)
        return DHT_BIT_TIMEOUT;
    bitCounter += 8;
    _byte2 = readByte(bitCounter);
    if (_byte2 == 0xFF)
        return DHT_BIT_TIMEOUT;
    bitCounter += 8;
    _byte3 = readByte(bitCounter);
    if (_byte3 == 0xFF)
        return DHT_BIT_TIMEOUT;
    bitCounter += 8;
    _byte4 = readByte(bitCounter);
    if (_byte4 == 0xFF)
        return DHT_BIT_TIMEOUT;
    bitCounter += 8;
    _byte5 = readByte(bitCounter);
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

DHTData MyDHT::getData(TempUnit unit)
{
    DHTData data;

    DHTError err = read();
    data.status = err;

    if (err != DHT_OK)
    {
        if (_hasLastValidData)
        {
            data = _lastValidData;
            data.status = err;
            return data;
        }

        data.temp = NAN;
        data.hum = NAN;
        data.dew = NAN;
        data.hi = NAN;
        return data;
    }

    data.temp = getTemperature(unit);
    data.hum = getHumidity();
    data.dew = getDewPoint(unit);
    data.hi = getHeatIndex(unit);

    _lastValidData = data;
    _hasLastValidData = true;

    return data;
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
int MyDHT::readOneBit(int counter)
{
    unsigned long t = micros();
    // Wait for LOW signal
    while (digitalRead(_pin) == HIGH)
    {
        if (micros() - t > _timings.bitTimeoutUs) // 120µs timeout
            return -1;
    }

    t = micros();
    // Measure LOW duration
    while (digitalRead(_pin) == LOW)
    {
        if (micros() - t > _timings.bitTimeoutUs) // 120µs timeout
            return -1;
    }
    unsigned long lowDuration = micros() - t;
    _lowTimes[counter] = lowDuration;

    unsigned long highStart = micros();
    // Start HIGH timer
    while (digitalRead(_pin) == HIGH)
    {
        if (micros() - highStart > _timings.bitTimeoutUs) // 120µs timeout
            return -1;
    }

    unsigned long highDuration = micros() - highStart;
    _highTimes[counter] = highDuration;
    // Determine bit value based on HIGH pulse duration
    if (highDuration > _timings.highThresholdUs)
        return 1;
    else
        return 0;
}

/*
  Read a byte (8 bits) from the sensor
  @return Byte value or 0xFF on error
*/
uint8_t MyDHT::readByte(int counter)
{
    uint8_t result = 0;

    for (int i = 0; i < 8; i++)
    {
        int bit = readOneBit(counter + i);

        if (bit < 0)
            return 0xFF;

        result <<= 1;
        result |= bit;
    }

    return result;
}

/*
  Get raw sensor bytes from the last read
  @return DHTRawData struct containing:
    - lowTimes[40]  : Duration of LOW pulses in microseconds
    - highTimes[40] : Duration of HIGH pulses in microseconds
    - bytes[5]      : Last 5 bytes read from sensor
*/
DHTRawData MyDHT::getRawData()
{
    DHTRawData data;
    // Store last 5 bytes read from sensor
    data.bytes[0] = _byte1;
    data.bytes[1] = _byte2;
    data.bytes[2] = _byte3;
    data.bytes[3] = _byte4;
    data.bytes[4] = _byte5;

    for (int i = 0; i < 40; i++)
    {
        data.highTimes[i] = _highTimes[i];
        data.lowTimes[i] = _lowTimes[i];
    }

    return data;
}

/*
  Starts an asynchronous read from the DHT sensor.
  @param cb User-provided callback function to be called when the read is complete.
  Sets the internal state to START_SIGNAL and begins the start signal sequence.
*/
void MyDHT::startAsyncRead(DHTCallback cb)
{
    _callback = cb;          // Store the user-defined callback function
    _state = START_SIGNAL;   // Set the state to START_SIGNAL to begin the start sequence
    pinMode(_pin, OUTPUT);   // Set the pin as OUTPUT to send the start signal
    digitalWrite(_pin, LOW); // Pull the pin LOW to signal the sensor to start sending data
    _timer = millis();       // Record the current time to measure start signal duration
}

/*
  Processes the asynchronous state machine.
  Should be called repeatedly (e.g., inside loop()) until isReading() returns false.
  Handles:
    - START_SIGNAL: Sends start signal to sensor
    - WAIT_ACK: Waits for sensor acknowledgment
    - READ_BITS_BLOCKING: Reads 5 bytes from sensor and calls the callback
    - ERROR_STATE: Handles errors and calls the callback with error status
*/
void MyDHT::processAsync()
{
    switch (_state)
    {
    case START_SIGNAL:
        // Wait for the start signal (LOW) to last at least 20ms
        if (millis() - _timer >= _timings.startLowMs)
        {
            digitalWrite(_pin, HIGH); // Pull the pin HIGH to finish the start signal
            delayMicroseconds(30);
            pinMode(_pin, INPUT_PULLUP); // Switch pin to INPUT_PULLUP to read sensor response
            _timer = micros();           // Record the start time for waiting ACK signal
            delayMicroseconds(80);
            _state = WAIT_ACK; // Change state to WAIT_ACK
        }
        break;

    case WAIT_ACK:
        // Wait for the sensor to pull the line LOW as ACK
        if (digitalRead(_pin) == LOW)
        {
            _state = READ_BITS_BLOCKING; // ACK received, move to reading bits
        }
        // Timeout: sensor did not respond
        else if (micros() - _timer > _timings.ackTimeoutUs)
        {
            _state = ERROR_STATE;
        }
        break;

    case READ_BITS_BLOCKING:
        DHTError e = read5Bytes(); // Read all 5 bytes from the sensor
        DHTData data = makeData();
        data.status = e;
        if (_callback)
            _callback(data); // Call user-defined callback with the data
        _state = IDLE;
        break;

    case ERROR_STATE:
        // Handle errors such as no response
        DHTData d;
        d.status = DHT_NO_RESPONSE;
        if (_callback)
            _callback(d); // Notify user via callback
        _state = IDLE;    // Reset state
        break;

    default:
        break;
    }
}

/*
  Checks if an asynchronous read is still in progress.
  @return true if a read is ongoing, false if idle
*/
bool MyDHT::isReading()
{
    return _state != IDLE;
}

/*
  Packages the last read raw bytes into a DHTData structure.
  Calculates temperature, humidity, dew point, and heat index.
  @return DHTData structure with all calculated values and status set to DHT_OK
*/
DHTData MyDHT::makeData(TempUnit unit)
{
    DHTData d;
    d.temp = getTemperature(unit);
    d.hum = getHumidity();
    d.dew = getDewPoint(unit);
    d.hi = getHeatIndex(unit);
    d.status = DHT_OK;
    return d;
}