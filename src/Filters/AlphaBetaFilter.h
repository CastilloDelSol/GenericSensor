#ifndef ALPHABETAFILTER_H
#define ALPHABETAFILTER_H

#include "BaseFilter.h"

class AlphaBetaFilter : public BaseFilter
{
private:
float &alpha() { return cfg.f[0]; }
float &beta() { return cfg.f[1]; }

    float _level;
    float _trend;
    bool _initialized;

public:
    AlphaBetaFilter(float a, float b) : _level(0.0f), _trend(0.0f), _initialized(false)
    {
        alpha() = a;
        beta() = b;
        setFilterType(FilterType::ALPHA_BETA);
    }

    ~AlphaBetaFilter() {}

    float apply(float value) override
    {
        if (!_initialized)
        {
            _level = value;
            _trend = 0;
            _initialized = true;
            return value;
        }

        float prev_level = _level;
        _level = alpha() * value + (1 - alpha()) * (_level + _trend);
        _trend = beta() * (_level - prev_level) + (1 - beta()) * _trend;

        return _level + _trend;
    }
};

#endif // ALPHABETAFILTER_H