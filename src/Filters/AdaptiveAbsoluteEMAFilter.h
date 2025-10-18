#ifndef ADAPTIVE_ABSOLUTE_EMA_FILTER_H
#define ADAPTIVE_ABSOLUTE_EMA_FILTER_H

#include "BaseFilter.h"
#include <Arduino.h>

class AdaptiveAbsoluteEMAFilter : public BaseFilter
{
private:
    float &alpha_min() { return cfg.f[0]; }
    float &delta_max() { return cfg.f[1]; }

    float alpha;            // Runtime adaptive alpha (not part of cfg)
    float prev_filtered;
    bool initialized;

public:
    AdaptiveAbsoluteEMAFilter(float a_min, float d_max)
        : alpha(a_min), prev_filtered(0.0f), initialized(false)
    {
        alpha_min() = a_min;
        delta_max() = d_max;
        setFilterType(FilterType::ADAPTIVE_ABSOLUTE_EMA); // Define in your enum
    }

    ~AdaptiveAbsoluteEMAFilter() {}

    float apply(float value) override
    {
        if (!initialized)
        {
            prev_filtered = value;
            initialized = true;
            return value;
        }

        float delta = fabs(value - prev_filtered);
        if (delta > delta_max())
            delta = delta_max();

        // Update adaptive alpha
        alpha = mapf(delta, 0.0f, delta_max(), alpha_min(), 1.0f);

        prev_filtered = alpha * value + (1.0f - alpha) * prev_filtered;

        return prev_filtered;
    }

    float getAlpha() const { return alpha; }

private:
    float mapf(float x, float in_min, float in_max, float out_min, float out_max)
    {
        if (x < in_min) x = in_min;
        if (x > in_max) x = in_max;
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
};

#endif // ADAPTIVE_ABSOLUTE_EMA_FILTER_H
