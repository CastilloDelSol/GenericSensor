#ifndef BASEFUNCTIONPROCESSOR_H
#define BASEFUNCTIONPROCESSOR_H

#include "BaseMapper.h"

class BaseFunctionProcessor : public BaseMapper
{
protected:
    enum FunctionType : uint8_t
    {
        NONE,
        POLYNOMIAL,
        RTD_CVD_385
    };

    void setFunctionType(FunctionType type) { cfg.u[POS_SUB_TYPE] = type; }

public:
BaseFunctionProcessor()
    {
        setMapperType(MapperType::FUNCTION);
    }

    ~BaseFunctionProcessor() {}
};

#endif // BASEFUNCTIONPROCESSOR_H