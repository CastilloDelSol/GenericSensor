#ifndef SENSOR_SPAN_H
#define SENSOR_SPAN_H

#include <Arduino.h>
#include "GenericSensor.h"

class SensorSpan {
public:
  SensorSpan() : _ptr(nullptr), _n(0) {}
  SensorSpan(GenericSensor* ptr, size_t n) : _ptr(ptr), _n(n) {}

  size_t size() const { return _n; }
  bool empty() const  { return _n == 0; }

  GenericSensor& operator[](size_t i)       { return _ptr[i]; }
  const GenericSensor& operator[](size_t i) const { return _ptr[i]; }

  GenericSensor& at(size_t i)       { return _ptr[i]; }
  const GenericSensor& at(size_t i) const { return _ptr[i]; }

  GenericSensor* data()       { return _ptr; }
  const GenericSensor* data() const { return _ptr; }

private:
  GenericSensor* _ptr;
  size_t _n;
};

#endif // SENSOR_SPAN_H