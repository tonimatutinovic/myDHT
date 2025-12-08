#include "MultiDHTManager.h"

MultiDHTManager::MultiDHTManager() : count(0) {}

void MultiDHTManager::addSensor(MyDHT &sensor)
{
    if (count < MAX_SENSORS)
    {
        sensors[count++] = &sensor;
    }
}

int MultiDHTManager::readAll(DHTResult results[])
{
    for (int i = 0; i < count; i++)
    {
        DHTError err = sensors[i]->read();
        results[i].pin = sensors[i]->getPin();
        results[i].temperature = (err == DHT_OK) ? sensors[i]->getTemperature(Celsius) : NAN;
        results[i].humidity = (err == DHT_OK) ? sensors[i]->getHumidity() : NAN;
        results[i].error = err;
    }
    return count;
}
