#ifndef MEDIAN3FILTER_H
#define MEDIAN3FILTER_H

#include "BaseFilter.h"

class Median3Filter : public BaseFilter
{
private:
    float values[3];
    uint8_t index;
    bool initialized;

public:
    Median3Filter() : values{0.0f, 0.0f, 0.0f}, index(0), initialized(false)
    {
        setFilterType(FilterType::MEDIAN3); // Add to your FilterType enum
    }

    ~Median3Filter() {}

    float apply(float value) override
    {
        values[index] = value;
        index = (index + 1) % 3;

        if (!initialized && index == 0)
            initialized = true;

        if (!initialized)
            return value;

        return median(values[0], values[1], values[2]);
    }

private:
    float median(float a, float b, float c)
    {
        if ((a >= b && a <= c) || (a >= c && a <= b)) return a;
        if ((b >= a && b <= c) || (b >= c && b <= a)) return b;
        return c;
    }
};

#endif // MEDIAN3FILTER_H
