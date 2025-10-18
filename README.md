# RTD-Processing Framework (v0.1.0)

A lightweight, modular C++ framework for precision sensor data processing — designed for microcontrollers (Arduino, ESP32, STM32) and laboratory-grade applications.  
Implements fast, numerically stable function mappers, filters, and measurement processors for resistive temperature devices (RTDs) and generic analog sensors.

---

## ✳️ Features

- **Callendar–Van Dusen RTD Processor**  
  - Supports any platinum RTD with α = 0.00385 (IEC 60751)  
  - Exact quadratic solution for T ≥ 0 °C  
  - Minimax-corrected single Newton step for T < 0 °C  
  - Universal normalization (`R/R₀`) → works with Pt100, Pt500, Pt1000  
  - Accuracy: full float accuracy.
  - Valid range: −200 °C … +661 °C  

- **Polynomial Mapper**  
  - Efficient Horner-form evaluation (supports any order)  
  - Coefficients defined as static `constexpr` arrays  
  - Useful for custom transfer functions or pre-calibrated sensors  

- **Filter Chain Framework**  
  - Generic `BaseFilter`, `EMAFilter`, and extensible design for multi-stage signal conditioning  
  - Low overhead, ideal for ADC streaming pipelines  

- **Generic Sensor Abstraction**  
  - Unified interface for mappers, filters, and processors  
  - Enables plug-and-play measurement pipelines  

---

## 🧮 RTD CVD 385 Processor

Header: `RTD_385.h`

```cpp
#include "RTD_385.h"

RTD385 rtd(100.0f);  // Pt100, default constants for α=0.00385

void loop() {
    float R = readADC();          // resistance in ohms
    float T = rtd.apply(R);       // temperature in °C
    Serial.println(T, 6);
}
```

**Internally:**
- For T ≥ 0 °C: exact quadratic inverse  
- For T < 0 °C: quadratic seed + minimax correction → two Newton steps 

---

## ⚙️ Structure

```
├── BaseFilter.h
├── BaseMapper.h
├── BaseMeasurementProcessor.h
├── BaseFunctionProcessor.h
├── PolynomialMapper.h
├── EMAFilter.h
├── GenericSensor.h
├── BaseTableProcessor.h
└── RTD_385.h
```

All modules are header-only and can be used independently.

---

## 📦 Installation

Copy the headers into your project or platform-specific `include/` directory, or use as an Arduino library:

```bash
git clone https://github.com/<your-repo-name>.git
```

Then include the desired modules:

```cpp
#include "RTD_385.h"
```

---

## 🧾 License

MIT License © 2025 Your Name  
See `LICENSE` for details.

---

## 🧭 Version History

| Version | Date | Notes |
|----------|------|-------|
| **v0.1.0** | 2025-10-18 | Initial public release — full CVD RTD processor, filter & mapper base classes |

---

**Precise. Efficient. Embedded-ready.**
