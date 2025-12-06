#ifndef MYDHTLIB_H
#define MYDHTLIB_H

#include <Arduino.h>

// Sensor type
enum DHTType
{
  DHT_AUTO,
  DHT11,
  DHT22
};

// Error codes returned by read()
enum DHTError
{
  DHT_OK = 0,        // Read successful
  DHT_NO_RESPONSE,   // Sensor did not respond to start signal
  DHT_ACK_TIMEOUT,   // Sensor ACK not received in time
  DHT_CHECKSUM_FAIL, // Checksum mismatch
  DHT_BIT_TIMEOUT    // Timeout while reading a bit
};

// Temperature units
enum TempUnit
{
  Celsius,
  Fahrenheit,
  Kelvin
};

// DHT raw data
struct DHTRawData
{
  uint8_t bytes[5];
  unsigned long highTimes[40];
  unsigned long lowTimes[40];
};

// DHT data
struct DHTData
{
  float temp;
  float hum;
  float dew;
  float hi;
  DHTError status;
};

// Async callback typedef
typedef void (*DHTCallback)(DHTData);

// Async states
enum DHTAsyncState
{
  IDLE,
  START_SIGNAL,
  WAIT_ACK,
  READ_BITS_BLOCKING,
  ERROR_STATE
};

// Timings
struct DHTTimings
{
  uint16_t startLowMs;
  uint16_t ackTimeoutUs;
  uint16_t ackDoneUs;
  uint16_t bitTimeoutUs;
  uint16_t highThresholdUs;
};

class MyDHT
{
public:
  /*
    Constructor
    @param pin   Arduino pin number
    @param type  Sensor type (DHT11 or DHT22)
    @param retries Number of retry attempts if read fails (default 3)
  */
  MyDHT(uint8_t pin, DHTType type = DHT_AUTO, uint8_t retries = 3);

  // Initialize the sensor (set pin mode, etc.)
  void begin();

  // Returns last read temperature in Celsius
  float getTemperature(TempUnit unit = Celsius);

  // Returns last relative humidity in %
  float getHumidity();

  /*
    Calculate dew point based on last read temperature and humidity
    Uses the _unit member to determine input temperature unit
    Returns dew point in the same unit as _unit
  */
  float getDewPoint(TempUnit unit = Celsius);

  /*
    Calculate heat index (feels-like temperature) based on last read temperature and humidity
    Internally converts temperature to Fahrenheit for formula, returns in same unit as _unit
  */
  float getHeatIndex(TempUnit unit = Celsius);

  // Read new data from sensor, returns DHTError
  DHTError read();

  // Set number of retry attempts if read fails
  void setRetries(uint8_t retries);

  // Adjust calibration offset
  void setTemperatureOffset(float offsetC);
  void setHumidityOffset(float offset);

  // Returns raw data read from the sensor
  DHTRawData getRawData();

  // Returns data package
  DHTData getData(TempUnit unit = Celsius);

  // Helper function to create a DHTData struct from the last read bytes
  DHTData makeData(TempUnit unit = Celsius);

  // Async API
  void startAsyncRead(DHTCallback cb); // Start an asynchronous sensor read
  void processAsync();                 // Must be called repeatedly (e.g., in loop()) to advance async read state machine
  bool isReading();                    // Returns true if an asynchronous read is currently in progress

  // Getter/Setter for sensor type
  DHTType getType();
  void setType(DHTType type);

  // Minimum wait time between two readings (depends on sensor type)
  uint16_t getMinReadInterval();

private:
  uint8_t _pin;             // Pin where sensor is connected
  DHTType _type = DHT_AUTO; // Sensor type
  uint8_t _retries;         // Number of retries
  DHTTimings _timings;      // Timing parameters depending on sensor type

  float _tempOffsetC = 0.0;    // Calibration offset
  float _humidityOffset = 0.0; // Humidity offset

  DHTData _lastValidData;         // Last successfully read sensor data
  bool _hasLastValidData = false; // Flag indicating if _lastValidData contains valid data

  // Low-level read functions
  int readOneBit(int counter);   // Reads a single bit from the sensor
  uint8_t readByte(int counter); // Reads a byte (8 bits) from the sensor
  DHTError readOnce();           // Performs a single read attempt
  DHTError read5Bytes();         // Reads 5 bytes from the sensor

  // Last read bytes from the sensor
  uint8_t _byte1, _byte2, _byte3, _byte4, _byte5;

  // Last read puls duration times
  unsigned long _highTimes[40];
  unsigned long _lowTimes[40];

  DHTAsyncState _state = IDLE;     // Current state of the asynchronous read state machine
  DHTCallback _callback = nullptr; // User-provided callback function for async read completion
  unsigned long _timer = 0;        // Timer used for measuring delays and timeouts in async reading

  void detectType(); // Detects sensor type DHT11/DHT22
};

#endif