#pragma once
#include <cmath>
#include <algorithm>

struct BlackScholesInputs {
    double S  = 100.0; // spot
    double K  = 100.0; // strike
    double T  = 1.0;   // years
    double v  = 0.2;   // volatility
    double r  = 0.05;  // risk-free rate
};

struct BlackScholesOutputs {
    double call = 0.0;
    double put  = 0.0;
    double call_delta = 0.0;
    double put_delta  = 0.0;
    double gamma      = 0.0;
};

inline double norm_pdf(double x) {
    static constexpr double INV_SQRT_2PI = 0.39894228040143267794;
    return INV_SQRT_2PI * std::exp(-0.5 * x * x);
}

// CDF via erf
inline double norm_cdf(double x) {
    return 0.5 * (1.0 + std::erf(x / std::sqrt(2.0)));
}

inline BlackScholesOutputs black_scholes_price(const BlackScholesInputs& in) {
    BlackScholesOutputs out{};

    const double S = std::max(in.S, 1e-12);
    const double K = std::max(in.K, 1e-12);
    const double T = std::max(in.T, 1e-12);
    const double v = std::max(in.v, 1e-12);
    const double r = in.r;

    const double sqrtT = std::sqrt(T);
    const double d1 = (std::log(S / K) + (r + 0.5 * v * v) * T) / (v * sqrtT);
    const double d2 = d1 - v * sqrtT;

    const double disc = std::exp(-r * T);

    out.call = S * norm_cdf(d1) - K * disc * norm_cdf(d2);
    out.put  = K * disc * norm_cdf(-d2) - S * norm_cdf(-d1);

    out.call_delta = norm_cdf(d1);
    out.put_delta  = out.call_delta - 1.0;

    // using the standard of pdf(d1)/(S*v*sqrtT)
    out.gamma = norm_pdf(d1) / (S * v * sqrtT);

    return out;
}

