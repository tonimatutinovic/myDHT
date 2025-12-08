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

    switch (_type)
    {
    case DHT11:
        _timings.startLowMs = 18;      // Pull LOW duration in ms
        _timings.ackTimeoutUs = 5000;  // ACK timeout in µs
        _timings.ackDoneUs = 80;       // ACK done threshold
        _timings.bitTimeoutUs = 120;   // Bit read timeout
        _timings.highThresholdUs = 50; // HIGH pulse threshold to distinguish 0/1
        break;

    case DHT22:
        _timings.startLowMs = 1;      // Pull LOW duration in ms
        _timings.ackTimeoutUs = 1000; // ACK timeout in µs
        _timings.ackDoneUs = 80;
        _timings.bitTimeoutUs = 80;
        _timings.highThresholdUs = 40;
        break;

    case DHT_AUTO:
    default:
        // Default safe timings for autodetect
        _timings.startLowMs = 18;
        _timings.ackTimeoutUs = 5000;
        _timings.ackDoneUs = 80;
        _timings.bitTimeoutUs = 120;
        _timings.highThresholdUs = 50;
        break;
    }

    // Initialising state last data
    _state = IDLE;
    _hasLastValidData = false;
    _lastError = DHT_OK;
    _failureCount = 0;
}

// Initialize the sensor (set pin mode)
void MyDHT::begin()
{
    pinMode(_pin, INPUT_PULLUP);

    // Delay for sensor stabilization
    delay(3000);

    if (_type == DHT_AUTO)
    {
        detectType(); // blocking read (~25ms or 50ms)
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
    // test mode
    if (testMode)
    {
        if (!sanityCheck()) // check simulated bytes
        {
            setError(DHT_ERROR_SANITY);
            return DHT_ERROR_SANITY;
        }
        _hasLastValidData = true;
        setError(DHT_OK);
        return DHT_OK;
    }

    DHTError err;
    uint16_t retryDelay = (_type == DHT11) ? 50 : 20;

    for (uint8_t attempt = 0; attempt < _retries; attempt++)
    {

        if (debugMode)
            debugPrint("Read attempt %d/%d", attempt + 1, _retries);

        err = readOnce();
        if (err == DHT_OK)
        {
            // If sanity check fails, report an error but fall back to last valid reading when available
            if (!sanityCheck())
            {
                setError(DHT_ERROR_SANITY);

                if (_hasLastValidData)
                {
                    return DHT_OK;
                }

                return DHT_ERROR_SANITY;
            }
            setError(DHT_OK);
            _failureCount = 0;
            _hasLastValidData = true;
            return DHT_OK;
        }

        if (debugMode)
            debugPrint("Read result: %s", getErrorString(err));

        delay(retryDelay); // Short delay before retry
    }

    setError(err);
    return err;
}

/*
  Perform a single read attempt from the sensor
  @return DHTError code
*/
DHTError MyDHT::readOnce()
{
    if (debugMode)
        debugPrint("Starting single read attempt on pin %d", _pin);

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
            setError(DHT_ERROR_NO_RESPONSE);
            return DHT_ERROR_NO_RESPONSE;
        }
    }

    timer = micros();
    while (digitalRead(_pin) == LOW)
    {
        if (micros() - timer > _timings.ackTimeoutUs)
        {
            setError(DHT_ERROR_TIMEOUT);
            return DHT_ERROR_TIMEOUT;
        }
    }

    timer = micros();
    while (digitalRead(_pin) == HIGH)
    {
        if (micros() - timer > _timings.ackTimeoutUs)
        {
            setError(DHT_ERROR_TIMEOUT);
            return DHT_ERROR_TIMEOUT;
        }
    }

    DHTError err = read5Bytes(); // Normal mode: read raw data from sensor

    if (debugMode)
        debugPrint("readOnce result: %s", getErrorString(err));

    return err;

    // For DHT_TEST_MODE example usage:
    // return DHT_OK;  // (see instructions in README)
}

/*
  Reads 5 bytes of data from the DHT sensor
  Internally calls readByte() for each byte and verifies the checksum
  @return DHT_OK on success, otherwise one of the DHTError codes:
        DHT_ERROR_BIT_TIMEOUT if a bit read timed out
        DHT_ERROR_CHECKSUM if checksum validation fails
*/
DHTError MyDHT::read5Bytes()
{
    int bitCounter = 0;

    // Read 5 bytes
    _byte1 = readByte(bitCounter);
    if (_byte1 == 0xFF)
    {
        setError(DHT_ERROR_BIT_TIMEOUT);
        return DHT_ERROR_BIT_TIMEOUT;
    }
    bitCounter += 8;
    _byte2 = readByte(bitCounter);
    if (_byte2 == 0xFF)
    {
        setError(DHT_ERROR_BIT_TIMEOUT);
        return DHT_ERROR_BIT_TIMEOUT;
    }
    bitCounter += 8;
    _byte3 = readByte(bitCounter);
    if (_byte3 == 0xFF)
    {
        setError(DHT_ERROR_BIT_TIMEOUT);
        return DHT_ERROR_BIT_TIMEOUT;
    }
    bitCounter += 8;
    _byte4 = readByte(bitCounter);
    if (_byte4 == 0xFF)
    {
        setError(DHT_ERROR_BIT_TIMEOUT);
        return DHT_ERROR_BIT_TIMEOUT;
    }
    bitCounter += 8;
    _byte5 = readByte(bitCounter);
    if (_byte5 == 0xFF)
    {
        setError(DHT_ERROR_BIT_TIMEOUT);
        return DHT_ERROR_BIT_TIMEOUT;
    }

    if (debugMode)
        debugPrint("Raw bytes: %X %X %X %X %X", _byte1, _byte2, _byte3, _byte4, _byte5);

    // Verify checksum
    uint8_t sum = _byte1 + _byte2 + _byte3 + _byte4;
    if (sum != _byte5)
    {
        setError(DHT_ERROR_CHECKSUM);
        if (debugMode)
            debugPrint("Checksum mismatch: %X != %X", sum, _byte5);
        return DHT_ERROR_CHECKSUM;
    }

    setError(DHT_OK);
    _failureCount = 0;
    if (debugMode)
        debugPrint("read5Bytes: OK, checksum verified");
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
            _state = WAIT_ACK;           // Change state to WAIT_ACK
        }
        break;

    case WAIT_ACK:
        if (micros() - _timer >= _timings.ackDoneUs)
        {
            // Wait for the sensor to pull the line LOW (after ACK)
            if (digitalRead(_pin) == LOW)
            {
                _state = READ_BITS_BLOCKING; // ACK received, move to reading bits
            }
            // Timeout: sensor did not respond
            else if (micros() - _timer > _timings.ackTimeoutUs)
            {
                _state = ERROR_STATE;
            }
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
        setError(DHT_ERROR_NO_RESPONSE);
        d.status = DHT_ERROR_NO_RESPONSE;
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

/*
  Attempts to automatically detect the sensor type (DHT11 or DHT22).
  Performs a single read to get raw bytes and applies a heuristic:
  - DHT11 typically has byte2 and byte4 equal to 0 and small temperature/humidity values.
  - DHT22 has non-zero decimal bytes or larger values.
  If the read fails, type remains DHT_AUTO for later retry.
*/
void MyDHT::detectType()
{
    // 1) Try reading as DHT22
    _type = DHT22;
    _timings = {1, 1000, 80, 80, 40};
    if (readOnce() == DHT_OK)
    {
        // Valid DHT22 frame → finished
        return;
    }

    // 2) Try reading as DHT11
    _type = DHT11;
    _timings = {18, 5000, 80, 120, 50};
    if (readOnce() == DHT_OK)
    {
        // Valid DHT11 frame → finished
        return;
    }

    // If neither worked
    _type = DHT_AUTO;
}

/*
  Getter for sensor type
*/
DHTType MyDHT::getType() { return _type; }

/*
  Setter for sensor type
*/
void MyDHT::setType(DHTType type) { _type = type; }

/*
  @return minimum recommended interval between reads (ms)
*/
uint16_t MyDHT::getMinReadInterval()
{
    return (_type == DHT11) ? 2000 : 1000; // ms
}

/*
  Returns the last error code occured
*/
DHTError MyDHT::getLastError() const { return _lastError; }

/*
  Returns the number of consecutive read failures
*/
uint16_t MyDHT::getFailureCount() const { return _failureCount; }

/*
  Returns true if the sensor appears to be connected (less than 5 consecutive failures)
*/
bool MyDHT::isConnected() const { return _failureCount < 5; }

/*
  Converts a DHTError code to a human-readable string.
  @param err The DHTError code
  @return const char* describing the error
*/
const char *MyDHT::getErrorString(DHTError err)
{
    switch (err)
    {
    case DHT_OK:
        return "OK";
    case DHT_ERROR_TIMEOUT:
        return "Timeout waiting for signal";
    case DHT_ERROR_CHECKSUM:
        return "Checksum mismatch";
    case DHT_ERROR_NO_RESPONSE:
        return "Sensor not responding";
    case DHT_ERROR_BIT_TIMEOUT:
        return "Timeout while reading a bit";
    case DHT_ERROR_INTERNAL:
        return "Unexpected internal failure";
    default:
        return "Unknown error";
    }
}

/*
  Performs validation of the decoded temperature and humidity values.
  Ensures that all readings fall within the allowed range for the sensor model.
  @return true if data is valid, false if values are unrealistic or out-of-range.
*/
bool MyDHT::sanityCheck()
{
    // DHTData object from the current raw bytes
    DHTData d = makeData(Celsius);

    if (debugMode)
        debugPrint("Sanity check: Temp=%f, Hum=%f", d.temp, d.hum);

    float temp = d.temp;
    float hum = d.hum;

    // Apply model-specific temperature bounds
    float minTemp = (_type == DHT11) ? DHT11_MIN_TEMP : DHT22_MIN_TEMP;
    float maxTemp = (_type == DHT11) ? DHT11_MAX_TEMP : DHT22_MAX_TEMP;

    // Reject invalid or out-of-range temperatures
    if (isnan(temp) || temp < minTemp || temp > maxTemp)
    {
        if (debugMode)
            debugPrint("Sanity check failed: Temperature=%f out of range", d.temp);
        return false;
    }

    // Reject invalid humidity (must be 0–100%)
    if (isnan(hum) || hum < 0.0f || hum > 100.0f)
    {
        if (debugMode)
            debugPrint("Sanity check failed: Humidity=%f out of range", d.hum);
        return false;
    }

    return true;
}

/*
  Injects raw sensor bytes for controlled testing without a physical DHT sensor.
  Used to verify decoding and sanity-check logic.
*/
void MyDHT::setRawBytes(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5)
{
    if (!testMode)
        return;

    _byte1 = b1;
    _byte2 = b2;
    _byte3 = b3;
    _byte4 = b4;
    _byte5 = b5;
}

/*
  Prints formatted debug messages to Serial with a "[DHT DEBUG]" prefix.
  Used for internal library debugging.

  Supports:
    - %f for floats
    - %d for integers
    - %s for C-strings
    - %X for hexadecimal integers
*/
void MyDHT::debugPrint(const char *fmt, ...)
{
    if (!debugMode)
        return;

    va_list args;
    va_start(args, fmt);

    char buf[256];    // Main buffer
    char tempBuf[32]; // Temp buffer for float or string conversion

    const char *traverse = fmt;
    char *bufPtr = buf;
    int remaining = sizeof(buf);

    while (*traverse && remaining > 0)
    {
        if (*traverse == '%' && *(traverse + 1) == 'f') // float
        {
            double f = va_arg(args, double);
            dtostrf(f, 5, 1, tempBuf); // width=5, 1 decimal place
            int n = snprintf(bufPtr, remaining, "%s", tempBuf);
            bufPtr += n;
            remaining -= n;
            traverse += 2;
        }
        else if (*traverse == '%' && *(traverse + 1) == 'd') // integer
        {
            int i = va_arg(args, int);
            int n = snprintf(bufPtr, remaining, "%d", i);
            bufPtr += n;
            remaining -= n;
            traverse += 2;
        }
        else if (*traverse == '%' && *(traverse + 1) == 's') // string
        {
            const char *s = va_arg(args, const char *);
            int n = snprintf(bufPtr, remaining, "%s", s);
            bufPtr += n;
            remaining -= n;
            traverse += 2;
        }
        else if (*traverse == '%' && *(traverse + 1) == 'X') // hex
        {
            int x = va_arg(args, int);
            int n = snprintf(bufPtr, remaining, "%02X", x); // always 2 digits
            bufPtr += n;
            remaining -= n;
            traverse += 2;
        }
        else // ordinary character
        {
            *bufPtr++ = *traverse++;
            remaining--;
        }
    }

    *bufPtr = '\0';
    va_end(args);

    Serial.print("[DHT DEBUG] ");
    Serial.println(buf);
}