#ifndef BASETABLEPROCESSOR_H
#define BASETABLEPROCESSOR_H

#include "BaseMapper.h"

class BaseTableProcessor : public BaseMapper
{
protected:
    enum TableType : uint8_t
    {
        NONE,
        PIECEWISE_LINEAR,
        CUBIC_SPLINE,
        CUBIC_HERMITE_MONOTONIC_SPLINE
    };

    static constexpr uint8_t MAX_TABLE_SIZE = 8;
    static constexpr uint8_t OFFSET_FX      = 8;

    inline float &x(uint8_t i) { return cfg.f[i]; }
    inline float &fx(uint8_t i) { return cfg.f[OFFSET_FX + i]; }

    inline uint8_t &tableSize() { return cfg.u[POS_TABLE_SIZE]; }

    void setTableType(TableType type) { cfg.u[POS_SUB_TYPE] = type; }

private:
    void sortAscending()
    {
        for (uint8_t i = 0; i < tableSize() - 1; i++)
        {
            for (uint8_t j = i + 1; j < tableSize(); j++)
            {
                if (x(j) < x(i))
                {
                    float temp = x(i);
                    x(i) = x(j);
                    x(j) = temp;

                    temp = fx(i);
                    fx(i) = fx(j);
                    fx(j) = temp;
                }
            }
        }
    }

public:
    BaseTableProcessor()
    {
        setMapperType(MapperType::TABLE);
    }

    ~BaseTableProcessor() {}

    bool pushPoint(const float xValue, const float fxValue)
    {
        if (tableSize() >= MAX_TABLE_SIZE)
        {
            return false;
        }

        x(tableSize()) = xValue;
        fx(tableSize()) = fxValue;

        tableSize()++;
        sortAscending();

        return true;
    }

    bool deletePoint(uint8_t idx)
    {
        if (idx >= tableSize())
        {
            return false;
        }

        for (uint8_t i = idx; i < tableSize() - 1; i++)
        {
            x(i) = x(i + 1);
            fx(i) = fx(i + 1);
        }

        x(tableSize() - 1) = 0.0f;
        fx(tableSize() - 1) = 0.0f;

        tableSize()--;

        return true;
    }

    float getX(uint8_t idx)
    {
        return (idx < tableSize()) ? x(idx) : 0.0f;
    }

    float getFX(uint8_t idx)
    {
        return (idx < tableSize()) ? fx(idx) : 0.0f;
    }
};

#endif // BASETABLEPROCESSOR_H