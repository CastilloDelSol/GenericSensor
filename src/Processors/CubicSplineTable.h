#ifndef CUBICSPLINETABLE_H
#define CUBICSPLINETABLE_H

#include "BaseTableProcessor.h"

class CubicSplineTable : public BaseTableProcessor
{
private:
	float interpolateCubicSpline(float value)
	{
		// tbd
		return value;
	}

public:
	CubicSplineTable() { setTableType(TableType::CUBIC_SPLINE); }
	~CubicSplineTable() {}

	// Performs cubic spline interpolation based on table data.
	float apply(float value) override
	{
		return interpolateCubicSpline(value);
	}
};

#endif // CUBICSPLINETABLE_H