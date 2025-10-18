#ifndef RTD_385_H
#define RTD_385_H

#include "BaseFunctionProcessor.h"
#include "PolynomialMapper.h"

/**
 * Input:  resistance in ohms (Ω) of a platinum RTD with α = 0.00385 (IEC 60751)
 * Output: temperature in °C
 *
 * Implements the Callendar–Van Dusen (CVD) equation:
 *   R(T) = R0 * (1 + A*T + B*T² + C*(T - 100)*T³)
 *
 * - For T ≥ 0 °C: uses the exact quadratic inverse (C = 0)
 * - For T < 0 °C: applies a fitted minimax correction to the quadratic seed,
 *   followed by two Newton–Raphson refinement on the full CVD model.
 *
 * Accuracy:
 *   ≤ 1.0e−7 °C for T ≥ 0 °C  (exact quadratic inverse, float-limited)
 *   ≤ 1.0e−7 °C for T < 0 °C  (minimax-corrected double Newton step, float limited)
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
 *   – All constants are user-adjustable via setR0(), setA(), setB(), and setC().
 *   – Works for any nominal RTD (Pt100, Pt500, Pt1000) with α = 0.00385.
 */
class RTD385 : public BaseFunctionProcessor
{
private:
    // Store constants in cfg.f[]
    inline float &R0() { return cfg.f[0]; }
    inline float &A()  { return cfg.f[1]; }
    inline float &B()  { return cfg.f[2]; }
    inline float &C()  { return cfg.f[3]; }

    // IEC 60751 default constants (α = 0.00385)
    static constexpr float R_RATIO_MIN = 0.185201f;  // R/R0 at -200°C
    static constexpr float R_RATIO_MAX = 3.904811f;  // R/R0 at +850°C
    static constexpr uint8_t NUM_COEFFS = 3;
    static constexpr float CVD_COEFFS[NUM_COEFFS] = {3.9083e-3f, -5.775e-7f, -4.183e-12f};
    static constexpr float SEED_COEFFS[NUM_COEFFS] = {2.5965757e-03f, 1.0078029e+00f, 9.4974362e-05f};
    static constexpr uint8_t NEWTON_STEPS = 2;

    inline float Rmin() { return R0() * R_RATIO_MIN; }
    inline float Rmax() { return R0() * R_RATIO_MAX; }

    float inv;

public:
    RTD385(float r0 = 100.0f)
    {
        R0() = r0;
        A() = CVD_COEFFS[0];
        B() = CVD_COEFFS[1];
        C() = CVD_COEFFS[2];
        setFunctionType(FunctionType::RTD_CVD_385);
        inv = 2.0f * B() * R0();
    }

    ~RTD385() {}

    float apply(float R) override
    {
        // Clamp resistance to valid IEC 60751 range
        R = constrain(R, Rmin(), Rmax());

        // --- Callendar–Van Dusen coefficients ---
        float a = B() * R0(), b = A() * R0(), c = R0() - R;
        float d = b * b - 4.0f * a * c;

        // ≥ 0 °C: exact solution
        //float T = (-b + sqrtf(d)) / (2.0f * a);
        float T = (-b + sqrtf(d)) * inv;
        if (R >= R0()) return T;

        // < 0 °C: refine quadratic seed using fitted correction, then one Newton step
        T = (SEED_COEFFS[2] * T + SEED_COEFFS[1]) * T + SEED_COEFFS[0];

        // < 0 °C: Newton-Raphson refinements on full CVD
        for (uint8_t i = 0; i < NEWTON_STEPS; ++i)
        {
            float T2 = T * T;
            float T3 = T2 * T;
            float f  = R0() * (1.0f + A() * T + B() * T2 + C() * (T - 100.0f) * T3) - R;
            float fp = R0() * (A() + 2.0f * B() * T + C() * T2 * (4.0f * T - 300.0f));
            T -= f / fp;
        }

        return T;
    }

    void setR0(float r0) { R0() = r0; inv = 2.0f * B() * R0(); }
};

/**
 * Input:  resistance R (Ω) of a platinum RTD with α = 0.00385 (IEC 60751)
 * Output: temperature in °C
 *
 * Implements the inverse Callendar–Van Dusen (CVD) equation:
 *   R(T) = R0 * (1 + A*T + B*T² + C*(T - 100)*T³)
 *
 * For T < 0 °C: uses a 7-degree minimax polynomial fit of the inverse.
 * For T ≥ 0 °C: uses the exact quadratic inverse of the simplified CVD (C = 0).
 *
 * Constants (IEC 60751, α = 0.00385):
 *   A = 3.9083e−3
 *   B = −5.775e−7
 *   C = −4.183e−12 (not used here)
 *
 * Max abs error of the negative-region polynomial:
 *   ≈ 4 × 10⁻⁶ °C (−200 … 0 °C)
 */
class RTD385Approximation : public PolynomialMapper
{
private:
    // Accessors for parameters stored in cfg.f[]
    inline float &R0()        { return cfg.f[8]; }
    inline float &InvR0()     { return cfg.f[9]; }
    inline float &A4R0()      { return cfg.f[10]; }   // 4 * A_coeff * R0
    inline float &A_coeff()   { return cfg.f[11]; }   // A * R0
    inline float &Inv2B()     { return cfg.f[12]; }   // 1 / (2 * B * R0)
    inline float &A_coeff2()  { return cfg.f[13]; }   // (A * R0)²

    static constexpr uint8_t NUM_COEFFS = 8;
    static constexpr float COEFFS_NEG[NUM_COEFFS] = {
        -2.4202168e+02f,  2.2230167e+02f,  2.5777109e+01f, -4.7150176e+00f,
        -2.7283411e+00f,  1.1117969e+00f,  4.1203939e-01f, -1.3757725e-01f
    };

    static constexpr float A_CVD = 3.9083e-3f;
    static constexpr float B_CVD = -5.775e-7f;
    static constexpr float C_CVD = -4.183e-12f;

public:
    explicit RTD385Approximation(float r0 = 100.0f)
    {
        setR0(r0);
        setDegree(NUM_COEFFS - 1);

        // Load negative-region polynomial coefficients
        for (uint8_t i = 0; i <= degree(); ++i) { c(i) = COEFFS_NEG[i]; }
    }

    ~RTD385Approximation() override = default;

    float apply(float R) override
    {
        const float r_ratio = R * InvR0();

        if (r_ratio < 1.0f)
        {
            // Negative region: minimax polynomial inverse
            return PolynomialMapper::apply(r_ratio);
        }
        else
        {
            // Positive region: exact quadratic inverse (C = 0)
            const float c_term = R0() - R;
            const float discriminant = A_coeff2() - A4R0() * c_term;
            return (-A_coeff() + sqrtf(discriminant)) * Inv2B();
        }
    }

    void setR0(float r0)
    {
        R0()       = r0;
        InvR0()    = 1.0f / r0;
        A_coeff()   = A_CVD * r0;               // A * R0
        A_coeff2()  = A_coeff() * A_coeff(); // (A * R0)²
        A4R0()      = 4.0f * B_CVD * r0;        // 4 * B * R0
        Inv2B()     = 1.0f / (2.0f * B_CVD * r0);
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