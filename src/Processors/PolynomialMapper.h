#ifndef POLYNOMIALMAPPER_H
#define POLYNOMIALMAPPER_H

#include "BaseFunctionProcessor.h"

class PolynomialMapper : public BaseFunctionProcessor
{
protected:
    inline float &c(uint8_t i) { return cfg.f[i]; }
    inline uint8_t &degree() {return cfg.u[POS_DEGREE]; }

public:
    PolynomialMapper()
    {
        // Default pass through f(x) = 1*x + 0
        degree() = 1;
        setCoefficient(0, 0.0f);
        setCoefficient(1, 1.0f);
        setFunctionType(FunctionType::POLYNOMIAL);
    }

    ~PolynomialMapper() {}

    float apply(float value) override
    {
        uint8_t deg = degree();

        float result = c(deg);

        for (int i = deg - 1; i >= 0; i--)
        {
            result = result * value + c(i);
        }

        return result;
    }

    uint8_t getDegree() { return degree(); }

    bool setDegree(uint8_t deg)
    {
        if (deg > 7) { return false; }
        degree() = deg;
        return true;
    }

    bool setCoefficient(uint8_t idx, float value)
    {
        if (idx > 8) { return false; }
        c(idx) = value;
        return true;
    }

    // Shortcut for setting f(x) = m*x + b
    void setLinear(float m, float b)
    {
        setDegree(1);        // linear, so degree = 1
        setCoefficient(0, b); // constant term
        setCoefficient(1, m); // slope term
    }

};

#endif // POLYNOMIALMAPPER_H