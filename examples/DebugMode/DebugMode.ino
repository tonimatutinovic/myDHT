#include <myDHTlib.h>

const int DHT_PIN = 2;          // Pin where DHT sensor is connected
MyDHT dht(DHT_PIN, DHT11);      // Create sensor object (DHT11 or DHT22)

void setup() {
    Serial.begin(115200);
    dht.begin();                 // Initialize sensor
    dht.debugMode = true;

    Serial.println("=== MyDHT Library DEBUG_MODE Example ===");
}

void loop() {
    Serial.println("\n--- Performing Read ---");

    // Perform a sensor read
    DHTError err = dht.read();

    if (err == DHT_OK) {
        // Print temperature, humidity, dew point, heat index
        Serial.print("Temperature: ");
        Serial.println(dht.getTemperature(Celsius), 2);
        Serial.print("Humidity: ");
        Serial.println(dht.getHumidity(), 2);
        Serial.print("Dew Point: ");
        Serial.println(dht.getDewPoint(Celsius), 2);
        Serial.print("Heat Index: ");
        Serial.println(dht.getHeatIndex(Celsius), 2);

        // Optional: get raw data for debugging
        DHTRawData raw = dht.getRawData();
        Serial.print("Raw bytes: ");
        for (int i = 0; i < 5; i++) {
            Serial.print(raw.bytes[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }
    else {
        // Print error
        Serial.print("Error: ");
        Serial.println(dht.getErrorString(err));

        if (!dht.isConnected()) {
            Serial.println("Sensor appears disconnected after multiple failures!");
        }
    }

    delay(5000); // Wait 5 seconds before next read
}
