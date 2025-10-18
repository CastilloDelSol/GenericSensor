#ifndef KALMANFILTER_H
#define KALMANFILTER_H

#include "BaseFilter.h"

class KalmanFilter : public BaseFilter
{
private:
    float &measurementNoise() { return cfg.f[0]; } // R
    float &processNoise()     { return cfg.f[1]; } // Q

    float estimate;
    float error_estimate;
    bool initialized;

public:
    KalmanFilter(float r, float q)
        : estimate(0.0f), error_estimate(1.0f), initialized(false)
    {
        measurementNoise() = r;
        processNoise() = q;
        setFilterType(FilterType::KALMAN); // Add this enum value to your system
    }

    ~KalmanFilter() {}

    float apply(float value) override
    {
        if (!initialized)
        {
            estimate = value;
            initialized = true;
            return value;
        }

        // Prediction update
        error_estimate += processNoise();

        // Measurement update
        float kalman_gain = error_estimate / (error_estimate + measurementNoise());
        estimate += kalman_gain * (value - estimate);
        error_estimate *= (1.0f - kalman_gain);

        return estimate;
    }
};

#endif // KALMANFILTER_H
