#include <myDHT.h>
#include <MultiDHTManager.h>

// Create sensors
MyDHT sensor1(2, DHT11);
MyDHT sensor2(3, DHT22);

// Create manager
MultiDHTManager manager;

DHTResult results[MAX_SENSORS];

void setup()
{
    Serial.begin(115200);

    sensor1.begin();
    sensor2.begin();

    // Optional: enable test mode if you want fake data
    // sensor1.testMode = true;
    // sensor2.testMode = true;

    // Add sensors to manager
    manager.addSensor(sensor1);
    manager.addSensor(sensor2);

    Serial.println("Multi-sensor demo started");
}

void loop()
{
    int n = manager.readAll(results);

    for (int i = 0; i < n; i++)
    {
        Serial.print("Sensor at pin ");
        Serial.print(results[i].pin);

        if (results[i].error == DHT_OK)
        {
            Serial.print(": Temp=");
            Serial.print(results[i].temperature);
            Serial.print("°C, RH=");
            Serial.println(results[i].humidity);
        }
        else
        {
            Serial.print(": Error=");
            Serial.println(results[i].error);
        }
    }

    Serial.println("--- Reading complete ---\n");
    delay(2000);
}
