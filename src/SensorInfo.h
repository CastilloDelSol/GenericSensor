#ifndef SENSORINFO_H
#define SENSORINFO_H

#include <Arduino.h>

struct SensorInfo {
    char manufacturer[32];
    char model[32];
    char serialNumber[16];
    char unit[8];        // e.g., "°C", "psi", "Pa"
    float lowerRange;     // Lower measuring range limit
    float upperRange;     // Upper measuring range limit

    SensorInfo() : lowerRange(0.0f), upperRange(0.0f)
    {
        manufacturer[0] = '\0';
        model[0] = '\0';
        serialNumber[0] = '\0';
        unit[0] = '\0';
    }
};

#endif // SENSORINFO_H
