#ifndef GENERIC_SENSOR_H
#define GENERIC_SENSOR_H

#include "BaseMeasurementProcessor.h"
#include "SensorInfo.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

class GenericSensor
{
private:
    mutable SemaphoreHandle_t _lock;

    static const uint8_t NUM_PROCESSORS = 5; // Total number of processors (mappers + filters)
    float processStageValue[NUM_PROCESSORS + 1];

public:
    BaseMeasurementProcessor *processor[NUM_PROCESSORS];
    
    GenericSensor() : _lock(nullptr)
    {
        if (_lock == nullptr)
        {
            _lock = xSemaphoreCreateMutex();
        }

        // Initialize processor array to nullptr
        for (int i = 0; i < NUM_PROCESSORS; i++)
        {
            processor[i] = nullptr;
        }

        // Initialize mappedValues array to 0.0f
        for (int i = 0; i < NUM_PROCESSORS + 1; i++)
        {
            processStageValue[i] = 0.0f;
        }
    }

    ~GenericSensor()
    {
        if (_lock)
        {
            vSemaphoreDelete(_lock);
        }
    }

    // Get the final processed value (after all processors are applied)
    float getReading()
    {
        float result = 0.0f;

        if (xSemaphoreTake(_lock, portMAX_DELAY) == pdTRUE)
        {
            result = processStageValue[NUM_PROCESSORS]; // Final processed value is at the last index
            xSemaphoreGive(_lock);
        }

        return result;
    }

    float* getProcessStagesValues()
    {
        float* stages = nullptr;

        if (xSemaphoreTake(_lock, portMAX_DELAY) == pdTRUE)
        {
            stages = processStageValue;
            xSemaphoreGive(_lock);
        }

        return stages;
    }

    void push(uint32_t x)
    {
        float startValueFloat = static_cast<float>(x);
        push(startValueFloat);
    }

    void push(uint16_t x)
    {
        float startValueFloat = static_cast<float>(x);
        push(startValueFloat);
    }

    void push(int32_t startValueInt)
    {
        float startValueFloat = static_cast<float>(startValueInt);
        push(startValueFloat);
    }

    // Unified push method to process a new value
    void push(float startValue)
    {
        if (xSemaphoreTake(_lock, portMAX_DELAY) == pdTRUE)
        {
            // Store the initial value at index 0 (before any processing)
            processStageValue[0] = startValue;

            // Apply all processors in sequence and store intermediate results in mappedValues
            for (int i = 0; i < NUM_PROCESSORS; i++)
            {
                processStageValue[i + 1] = (processor[i]) ? processor[i]->apply(processStageValue[i]) : processStageValue[i];
            }

            xSemaphoreGive(_lock);
        }
    }

    // Setters for mappers and filters with clear validation
    void setMapper(uint8_t idx, BaseMeasurementProcessor *proc)
    {
        if (idx < 3) // Mappers are indices 0, 1, 2
        {
            processor[idx] = proc;
        }
    }

    void setFilter(uint8_t idx, BaseMeasurementProcessor *proc)
    {
        if (idx < 2) // Filters are indices 3 and 4
        {
            processor[idx + 3] = proc; // Offset the filter indices by 3
        }
    }

private:
    void setProcessor(uint8_t idx, BaseMeasurementProcessor *proc)
    {
        processor[idx] = proc;
    }
};

#endif // GENERIC_SENSOR_H
