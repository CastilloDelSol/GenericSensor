#ifndef RTD_385_H
#define RTD_385_H

#include "PolynomialMapper.h"

/**
 * Input:  resistance in ohms (Ω) of a platinum RTD with α = 0.00385 (IEC 60751)
 * Output: temperature in °C
 *
 * Implements the Callendar–Van Dusen (CVD) equation:
 *   R(T) = R0 * (1 + A*T + B*T² + C*(T - 100)*T³)
 *
 * - For T ≥ 0 °C: uses the exact quadratic inverse (C = 0)
 * - For T < 0 °C: applies a Remez minimax 7th degree polynomial
 *
 * Valid range (IEC 60751): −200 °C to +661 °C
 *
 * Constants (IEC 60751, α = 0.00385):
 *   R0 = 100.0 Ω  (nominal resistance at 0 °C; set 1000.0 for Pt1000, etc.)
 *   A  = 3.9083e−3
 *   B  = −5.775e−7
 *   C  = −4.183e−12
 *
 * Notes:
 *   – Uses a normalized resistance ratio R/R0 for scaling.
 *   – Adjust R0 via setR0().
 *   – Works for any nominal RTD (Pt100, Pt500, Pt1000) with α = 0.00385.
 */
class RTD385 : public PolynomialMapper
{
public:
    explicit RTD385(float r0 = 100.0f)
    { 
        // Initialize Callendar–Van Dusen constants
        R0() = r0;
        A()  = CVD_A;
        B()  = CVD_B;
        C()  = CVD_C;

        setR0(r0);
        setDegree(NUM_COEFFS - 1);

        // Load negative-region polynomial coefficients
        for (uint8_t i = 0; i <= degree(); ++i) { c(i) = COEFFS_NEG[i]; }

        setFunctionType(FunctionType::RTD_CVD_385);
    }

    // Set R0 and recalculate all precomputed coefficients based on R0.
    void setR0(float r0)
    {
        R0()       = r0;
        invR0      = 1.0f / R0();
        b          = A() * R0();                   // linear coefficient (A·R0)
        inv2a      = 1.0f / (2.0f * B() * R0());   // reciprocal of 2a = 2B·R0
        b_squared  = b * b;                        // cache b²
        a4         = 4.0f * B() * R0();            // 4a = 4B·R0
    }

    float apply(float R) override
    {
        // Normalize and clamp to valid range
        float r = constrain(R * invR0, RATIO_MIN, RATIO_MAX);
        return (r < 1.0f) ? PolynomialMapper::apply(r) : T_from_r_pos(r);
    }

private:
    // Accessors into cfg storage
    inline float &R0() { return cfg.f[8]; }
    inline float &A()  { return cfg.f[9]; }
    inline float &B()  { return cfg.f[10]; }
    inline float &C()  { return cfg.f[11]; }

    // Precomputed values for positive-branch inversion
    float b;           // A·R0
    float inv2a;       // 1 / (2·B·R0)
    float b_squared;   // (A·R0)²
    float a4;          // 4·B·R0
    float invR0;       // 1 / R0

    // ---- IEC 60751 constants ----
    static constexpr float CVD_A =  3.9083e-3f;
    static constexpr float CVD_B = -5.775e-7f;
    static constexpr float CVD_C = -4.183e-12f;

    // Normalized R/R0 range for −200 … +850 °C
    static constexpr float RATIO_MIN = 0.1852f;
    static constexpr float RATIO_MAX = 3.33106f;

    static constexpr uint8_t NUM_COEFFS = 8;
    static constexpr float COEFFS_NEG[NUM_COEFFS] = {
        -2.4202168e+02f,  2.2230167e+02f,  2.5777109e+01f, -4.7150176e+00f,
        -2.7283411e+00f,  1.1117969e+00f,  4.1203939e-01f, -1.3757725e-01f
    };

    // ---- Positive branch: analytic quadratic solution ----
    inline float T_from_r_pos(float r)
    {
        // Express in Ω-domain for better float precision near 0 °C
        float c = R0() * (1.0f - r);     // (R0 − R)
        float d = b_squared - a4 * c;    // discriminant b² − 4a·c
        return (-b + sqrtf(d)) * inv2a;
    }
};

/**
 * Input:  resistance in ohms (Ω) of a platinum RTD with α = 0.00385 (IEC 60751)
 * Output: temperature in °C
 *
 * Implements a 2nd-degree minimax polynomial approximation of the IEC 60751
 * inverse Callendar–Van Dusen (CVD) equation, valid for:
 *     Range: +5 °C … +45 °C  (101.953 Ω … 117.470 Ω)
 *
 * Model:
 *     T(R) ≈ c0 + c1·R + c2·R²
 *
 * Fit details:
 *     Degree: 2
 *     Method: Minimax polynomial fit of CVD inverse
 *     Maximum absolute error: 8.86 × 10⁻⁵ °C
 *
 * This polynomial provides a compact, high-accuracy approximation suitable for
 * embedded or resource-constrained systems where the full CVD equation would be
 * too costly to evaluate.
 *
 * Example usage:
 *     RTD385_5C45C_PT100 map;
 *     float tempC = map.apply(resistance_ohms);
 */
class RTD385_5C45C_PT100 : public PolynomialMapper {
private:
    static constexpr uint8_t NUM_COEFFS = 3;
    static constexpr float COEFFS[NUM_COEFFS] = {-2.4595622734e+02f, 2.3606534620e+00f, 9.8912416502e-04f};

public:
    RTD385_5C45C_PT100() noexcept
    {
        setDegree(NUM_COEFFS-1);
        for (uint8_t i = 0; i < NUM_COEFFS; ++i) { setCoefficient(i, COEFFS[i]); }
    }

    ~RTD385_5C45C_PT100() = default;
};

/**
 * Input:  resistance in ohms (Ω) of a platinum RTD with α = 0.00385 (IEC 60751)
 * Output: temperature in °C
 *
 * Implements a 4th-degree minimax polynomial approximation of the IEC 60751
 * inverse Callendar–Van Dusen (CVD) equation, valid for:
 *     Range: −50 °C … +120 °C  (80.306 Ω … 146.068 Ω)
 *
 * Model:
 *     T(R) ≈ c0 + c1·R + c2·R² + c3·R³ + c4·R⁴
 *
 * Fit details:
 *     Degree: 4
 *     Method: Minimax polynomial fit of CVD inverse
 *     Maximum absolute error: 9.08 × 10⁻⁴ °C
 *
 * Accuracy is limited only by the polynomial approximation;
 * coefficients are constants derived from the CVD model for α = 0.00385.
 *
 * Example usage:
 *     RTD385_N50C120C_PT100 map;
 *     float tempC = map.apply(resistance_ohms);
 */
class RTD385_N50C120C_PT100 : public PolynomialMapper {
private:
    static constexpr uint8_t NUM_COEFFS = 5;
    static constexpr float COEFFS[NUM_COEFFS] = {-2.4407078132e+02f, 2.2886580349e+00f, 2.0127370843e-03f, -6.4360670490e-06f, 1.5127721577e-08f};

public:
    RTD385_N50C120C_PT100() noexcept
    {
        setDegree(NUM_COEFFS - 1);
        for (uint8_t i = 0; i < NUM_COEFFS; ++i) { setCoefficient(i, COEFFS[i]); }
    }

    ~RTD385_N50C120C_PT100() = default;
};


#endif // RTD_385_H
