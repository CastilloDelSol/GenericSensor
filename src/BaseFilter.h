#ifndef BASEFILTER_H
#define BASEFILTER_H

#include "BaseMeasurementProcessor.h"

class BaseFilter : public BaseMeasurementProcessor
{
protected:
    enum FilterType : uint8_t
    {
        NONE = 0,           // No filter / passthrough
        EXP_MOVING_AVERAGE, // Exponential Moving Average
        ALPHA_BETA,          // Alpha-Beta filter (simple predictor)
        ADAPTIVE_ABSOLUTE_EMA,
        KALMAN,
        MEDIAN3
    };

    void setFilterType(FilterType type) { cfg.u[POS_SUB_TYPE] = type; }

public:
    BaseFilter() { setProcessorType(ProcessorType::FILTER); }

    // Default passthrough apply
    float apply(float value) override { return value; }
};

#endif // BASEFILTER_H