#ifndef IMEASUREMENTPROCESSOR_H
#define IMEASUREMENTPROCESSOR_H

#include <Arduino.h>

/*
IMeasurementProcessor (interface)
├── BaseMapper (generic transformation interface + config)
│   ├── BaseTableProcessor (shared logic for table-based transforms)
│   │   ├── PiecewiseLinearTable
│   │   └── CubicSplineTable
│   └── PolynomialMapper
└── BaseFilter (generic filter base class + config)
	├── EMAFilter
	├── SMAFilter
	├── AlphaBetaFilter
	├── ButterworthFilter
	├── Kalman1DFilter
	├── LowPassFilter
	├── HighPassFilter
	├── BiquadFilter
	├── MedianFilter
	├── WMAFilter
	└── FIRFilter
*/
struct ProcessorConfig
{
	uint8_t u[8]{};
	uint32_t unit[2];
	float f[16]{};
};

class BaseMeasurementProcessor
{
protected:
	enum ProcessorType : uint8_t
	{
		NONE = 0,
		MAPPER,
		FILTER
	};

	ProcessorConfig cfg;

	static constexpr uint8_t POS_PROCESSOR_TYPE		= 0;
	static constexpr uint8_t POS_MAPPER_TYPE 		= 1;
	static constexpr uint8_t POS_SUB_TYPE			= 2;
	static constexpr uint8_t POS_TABLE_SIZE			= 3;
	static constexpr uint8_t POS_DEGREE				= 4;


	void setProcessorType(ProcessorType type) { cfg.u[POS_PROCESSOR_TYPE] = type; }

public:
	BaseMeasurementProcessor()
	{
		for (int i = 0; i < 16; i++)
		{
			cfg.f[i] = 0.0f;
		}

		for (int i = 0; i < 8; i++)
		{
			cfg.u[i] = 0;
		}

		for (int i = 0; i < 2; i++)
		{
			cfg.unit[i] = 0;
		}

	}

	virtual ~BaseMeasurementProcessor() {}
	virtual float apply(float value) = 0;

	void setFloat(uint8_t idx, float f)
	{
		idx = constrain(idx, 0, 15);
		cfg.f[idx] = f;
	}

	void setByte(uint8_t idx, uint8_t u)
	{
		idx = constrain(idx, 0, 11);
		cfg.u[idx] = u;
	}

	void setUnits(uint8_t idx, uint32_t packedUnits){
		cfg.unit[idx] = packedUnits;
	}
};

#endif // IMEASUREMENTPROCESSOR_H