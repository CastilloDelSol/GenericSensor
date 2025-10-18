#ifndef CUBICHERMITEMONOTONICSPLINE_H
#define CUBICHERMITEMONOTONICSPLINE_H

#include "BaseTableProcessor.h"

class CubicHermiteMonotonicSplineTable : public BaseTableProcessor
{
private:
    // Array to store the computed derivative (slope) at each data point.
    float _m[MAX_TABLE_SIZE];

    void updateSlopes()
    {
        // Get the current number of data points in the table.
        uint8_t size = tableSize();

        // --- Compute segment slopes (delta) between consecutive points ---
        float delta[size - 1];
        for (uint8_t i = 0; i < size - 1; i++)
        {
            delta[i] = (fx(i + 1) - fx(i)) / (x(i + 1) - x(i));
        }

        // --- Compute derivatives at each point using the PCHIP formula ---
        // For the endpoints, use the slope of the first (or last) segment.
        _m[0] = delta[0];
        _m[size - 1] = delta[size - 2];

        // Determine others as harmonic mean or set to 0
        for (uint8_t i = 1; i < size - 1; i++)
        {
            if (delta[i - 1] * delta[i] > 0)
            {
                _m[i] = 2 * delta[i - 1] * delta[i] / (delta[i - 1] + delta[i]);
            }
            else
            {
                _m[i] = 0;
            }
        }
    }

    float splineInterpolation(float value)
    {
        // Get the number of data points.
        uint8_t size = tableSize();

        // --- Boundary Handling ---
        if (value <= x(0))
        {
            return fx(0);
        }

        if (value >= x(size - 1))
        {
            return fx(size - 1);
        }

        // --- Find the correct interval --- (linear search)
        uint8_t pos = 1;
        while (value > x(pos))
        {
            pos++;
        }

        // --- Compute normalized distance within the segment ---
        // h is the interval length, and t is the normalized parameter [0, 1].
        float h = x(pos) - x(pos - 1);
        float t = (value - x(pos - 1)) / h;
        float t2 = t * t;
        float t3 = t2 * t;

        // --- Hermite Basis Functions ---
        // h00 and h01 blend the endpoint values; h10 and h11 blend the tangents.
        float h00 = 2 * t3 - 3 * t2 + 1; // Basis function for the first endpoint
        float h10 = t3 - 2 * t2 + t;     // Basis function for the first tangent
        float h01 = -2 * t3 + 3 * t2;    // Basis function for the second endpoint
        float h11 = t3 - t2;             // Basis function for the second tangent

        // Retrieve the function values (y0 and y1) at the interval endpoints.
        float y0 = fx(pos - 1);
        float y1 = fx(pos);
        // Scale the derivatives by the interval length to obtain tangent contributions.
        float m0 = _m[pos - 1] * h;
        float m1 = _m[pos] * h;

        return h00 * y0 + h10 * m0 + h01 * y1 + h11 * m1;
    }

public:
    // Constructor: Set the table type and initialize the derivative array to zero.
    CubicHermiteMonotonicSplineTable()
    {
        setTableType(TableType::CUBIC_HERMITE_MONOTONIC_SPLINE);

        for (int i = 0; i < MAX_TABLE_SIZE; i++)
        {
            _m[i] = 0.0f;
        }
    }

    ~CubicHermiteMonotonicSplineTable() {}

    bool pushPoint(const float xValue, const float fxValue)
    {
        bool success = BaseTableProcessor::pushPoint(xValue, fxValue);

        if (success)
        {
            updateSlopes();
        }

        return success;
    }

    float apply(float value) override
    {
        if (tableSize() < 2)
        {
            return fx(0);
        }

        return splineInterpolation(value);
    }
};

#endif // CUBICHERMITEMONOTONICSPLINE_H