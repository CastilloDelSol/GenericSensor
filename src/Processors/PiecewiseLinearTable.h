#ifndef PIECEWISELINEARTABLE_H
#define PIECEWISELINEARTABLE_H

#include "BaseTableProcessor.h"

class PiecewiseLinearTable : public BaseTableProcessor
{
private:
    inline float piecewiseLinearInterpolation(float value)
    {
        if (tableSize() < 2)
        {
            return fx(0);
        }

        /*
        if (value <= x(0))
        {
            return fx(0);
        }

        if (value >= x(tableSize() - 1))
        {
            return fx(tableSize() - 1);
        }
            */

        uint8_t pos = 1;
        while (value > x(pos) && pos < tableSize() - 1)
        {
            pos++;
        }

        if (value == x(pos))
        {
            return fx(pos);
        }

        return (value - x(pos - 1)) * (fx(pos) - fx(pos - 1)) / (x(pos) - x(pos - 1)) + fx(pos - 1);
    }

public:
    PiecewiseLinearTable() { setTableType(TableType::PIECEWISE_LINEAR); }
    ~PiecewiseLinearTable() {}

    float apply(float value) override
    {
        return piecewiseLinearInterpolation(value);
    }
};

#endif // PIECEWISELINEARTABLE_H