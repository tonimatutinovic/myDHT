#ifndef MULTIDHTMANAGER_H
#define MULTIDHTMANAGER_H

#include "myDHTlib.h"

#define MAX_SENSORS 8

struct DHTResult
{
    int pin;
    float temperature;
    float humidity;
    DHTError error;
};

class MultiDHTManager
{
public:
    MultiDHTManager();

    void addSensor(MyDHT &sensor);
    int readAll(DHTResult results[]); // returns number of sensors read

private:
    MyDHT *sensors[MAX_SENSORS];
    int count;
};

#endif