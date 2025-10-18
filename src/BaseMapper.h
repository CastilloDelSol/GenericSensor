#ifndef BASEMAPPER_H
#define BASEMAPPER_H

#include "BaseMeasurementProcessor.h"

class BaseMapper : public BaseMeasurementProcessor
{
protected:
    enum MapperType : uint8_t
    {
        NONE = 0,
        TABLE,
        FUNCTION
    };

    void setMapperType(MapperType type) { cfg.u[POS_MAPPER_TYPE] = type; }

public:
    BaseMapper() { setProcessorType(ProcessorType::MAPPER); }

    // Default passthrough apply
    float apply(float value) override { return value; }
};

#endif // BASEMAPPER_H