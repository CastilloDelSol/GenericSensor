#ifndef EMAFILTER_H
#define EMAFILTER_H

#include "BaseFilter.h"

class EMAFilter : public BaseFilter
{
private:
    float &alpha() { return cfg.f[0]; }

    bool _initialized;
    float _ema;

    void prime(float x)
    {
        _initialized = true;
        _ema = x;
    }

public:
    EMAFilter(float a = 1.0f) : _initialized(false), _ema(0.0f)
    {
        alpha() = a;
        setFilterType(FilterType::EXP_MOVING_AVERAGE);
    }

    ~EMAFilter() {}

    float apply(float value) override
    {
        if (!_initialized)
        {
            //_initialized = true;
            //_ema = value;
            prime(value);
        }
        else
        {
            _ema = _ema * (1.0f - alpha()) + value * alpha();
        }

        return _ema;
    }

    void setAlpha(float a)
    {
        a = constrain(a, std::nextafter(0.0f,1.0f), 1.0f);
        alpha() = a;
    }
};
#endif // EMAFILTER_H