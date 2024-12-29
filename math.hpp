#pragma once

#include <algorithm>
#include <cmath>

#include "./fixed.hpp"
#include "./const.hpp"
#include "./math_helper.hpp"

namespace fxd {

// Manipulation

template<std::integral base, int fp>
constexpr fixed<base, fp> abs(fixed<base, fp> s) {
    if constexpr(fixed<base, fp>::is_signed)
        return s < 0 ? -s : s;
    else
        return s;
}

template<std::integral base, int fp>
constexpr int sign(fixed<base, fp> s) {
    if constexpr(fixed<base, fp>::is_signed)
        return s > 0 ? 1 : (s < 0 ? -1 : 0);
    else
        return s > 0 ? 1 : 0;
}

template<std::integral base, int fp>
constexpr fixed<base, fp> trunc(fixed<base, fp> s) {
    if constexpr(fixed<base, fp>::is_signed)
        return fixed<base, fp>(s.raw() / (base(1) << fp));
    else
        return fixed<base, fp>::from_raw(s.raw() & fixed<base, fp>::int_mask);
}

template<std::integral base, int fp>
constexpr fixed<base, fp> ceil(fixed<base, fp> s) {
    if constexpr(fixed<base, fp>::is_signed) {
        if (s > 0) 
            s += fixed<base, fp>::max_fraction();
        return trunc(s);
    }
    else {
        return trunc(s + fixed<base, fp>::max_fraction());
    }
}

template<std::integral base, int fp>
constexpr fixed<base, fp> floor(fixed<base, fp> s) {
    if constexpr(fixed<base, fp>::is_signed) {
        if (s < 0) 
            s -= fixed<base, fp>::max_fraction();
        return trunc(s);
    }
    else {
        return trunc(s);
    }
}

template<std::integral base, int fp>
constexpr fixed<base, fp> round(fixed<base, fp> s) {
    if constexpr(fixed<base, fp>::is_signed) {
        const fixed<base, fp> half = (s > 0) ? 0.5 : -0.5;
        return trunc(s + half);
    }
    else {
        return trunc(s + 0.5);
    }
}

template<std::integral base, int fp>
constexpr fixed<base, fp> clamp(fixed<base, fp> x, decltype(x) min = 0, decltype(x) max = 1) {
    return (x < min) ? min : ((x > max) ? max : x);
}

template<std::integral base, int fp>
constexpr fixed<base, fp> lerp(fixed<base, fp> a, fixed<base, fp> b, decltype(a) t) {
    return a + (b - a) * t;
}

template<std::integral base, int fp>
constexpr fixed<base, fp> smoothstep(fixed<base, fp> x, decltype(x) edge0, decltype(x) edge1) {
    using fixed_t = fixed<base, fp>;

    x = clamp((x - edge0) / (edge1 - edge0));
    return x * x * (3 - (x << 1));
}

// Exponents

template<std::integral base, int fp>
constexpr fixed<base, fp> sqrt(fixed<base, fp> s) {
    using fixed_t = fixed<base, fp>;

    if (s <= 0)
        return 0;

    const int log2 = impl::ilog2(s.raw()) - fp;
    const high_t x = ((log2 > 0) ? (s >> log2) : (s << -log2)) >> 1;
    const int idx = (x & (0xfe << 17)).raw() >> 18;

    high_t y = high_t::from_raw(impl::rsqrt_lut[idx] >> 5);
    y *= 1.5 - (x * y * y);
    y *= 1.5 - (x * y * y);

    y *= (x << 1);

    if (log2 & 1)
        y *= (log2 > 0) ? sqrt_2<high_t> : rsqrt_2<high_t>;

	// TODO: Maybe not lose precision here? Cast to a wider type?
    const fixed_t out = y;
    return (log2 > 0) ? (out << (log2 >> 1)) : (out >> (-log2 >> 1));
}

template<std::integral base, int fp>
constexpr fixed<base, fp> rsqrt(fixed<base, fp> s) {
    using fixed_t = fixed<base, fp>;

    if (s <= 0)
        return 0;

    const int log2 = impl::ilog2(s.raw()) - fp;
    const high_t x = ((log2 > 0) ? (s >> log2) : (s << -log2)) >> 1;
    const int idx = (x & (0xfe << 17)).raw() >> 18;

    high_t y = high_t::from_raw(impl::rsqrt_lut[idx] >> 5);
    y *= 1.5 - (x * y * y);
    y *= 1.5 - (x * y * y);

    if (log2 & 1)
        y *= (log2 > 0) ? rsqrt_2<high_t> : sqrt_2<high_t>;

    const fixed_t out = y;
    return (log2 > 0) ? (out >> (log2 >> 1)) : (out << (-log2 >> 1));
}

template<std::integral base, int fp>
constexpr fixed<base, fp> rcp(fixed<base, fp> s) {
    using fixed_t = fixed<base, fp>;

    if (s == 0)
        return fixed_t::max();

    if (s < 0)
        return -rcp(-s);

    const int log2 = impl::ilog2(s.raw()) - fp;
    const high_t x = (log2 > 0) ? (s >> log2) : (s << -log2);
    const int idx = (x & (0xfe << 18)).raw() >> 19;

    high_t y = high_t::from_raw(impl::rcp_lut[idx] >> 5);
    y *= (2 - x * y);
    y *= (2 - x * y);

    fixed_t out = y;
    return (log2 > 0) ? (out >> log2) : (out << -log2);
}

// Extended reciprocal for max precision.
// All inputs MUST be above 1.

template<std::integral base, int fp>
constexpr frac_t rcp_ext(fixed<base, fp> s) {
    if (s == 0)
        return frac_t::max();

    if (s < 0)
        return -rcp_ext(-s);

    if (s < 1)
        return 0;

    const int log2 = impl::ilog2(s.raw()) - fp;
    const frac_t x = s >> log2;
    const int idx = (x & (0xfe << 20)).raw() >> 21;

    frac_t y = frac_t::from_raw(impl::rcp_lut[idx] >> 3);
    y *= (2 - x * y);
    y *= (2 - x * y);

    return y >> log2;
}

template<std::integral base, int fp>
constexpr fixed<base, fp> hypot(fixed<base, fp> x, fixed<base, fp> y) {
    return sqrt(x * x + y * y);
}

template<std::integral base, int fp>
constexpr fixed<base, fp> hypot(fixed<base, fp> x, fixed<base, fp> y, fixed<base, fp> z) {
    return sqrt(x * x + y * y + z * z);
}

// Logarithms

template<std::integral base, int fp, bool highp = true>
constexpr exp_t log2(fixed<base, fp> s) {
    using impl::log_t;
    
    if (s <= 0)
        return exp_t::min();

    const int log2 = impl::ilog2(s.raw()) - fp;

    const log_t x = impl::log2_sqrt((log2 > 0) ? (s >> log2) : (s << -log2));
    constexpr log_t c1 =  0.11598391789742051872;
    constexpr log_t c2 = -0.87468467888034107105;
    constexpr log_t c3 =  2.80670941352567426819;
    constexpr log_t c4 = -5.05195558416252410439;
    constexpr log_t c5 =  6.04525303940652847245;
    constexpr log_t c6 = -3.04130610778675780637;
    const log_t y = x * (x * (x * (x * (c1 * x + c2) + c3) + c4) + c5) + c6;
    return log2 + static_cast<exp_t>(y << 1);
}

template<std::integral base, int fp>
constexpr exp_t log(fixed<base, fp> s) {
    return log2(s) * ln2<exp_t>;
}

template<std::integral base, int fp>
constexpr exp_t log10(fixed<base, fp> s) {
    return log2(s) * log10_2<exp_t>;
}

// Powers

template<std::integral base, int fp>    
constexpr fixed<base, fp> exp2(fixed<base, fp> s, exp_t multiplier = 1.0) {
    using fixed_t = fixed<base, fp>;    

    if (s == 0)
        return 1;

    constexpr fixed_t max_exp = log2(fixed_t::max());
    constexpr fixed_t min_exp = impl::get_min_exp2_input<base, fp>();
    if (s >= max_exp)
        return fixed_t::max();
    if (s <= min_exp)
        return fixed_t::min_frac();

    auto approx = [] (exp_t x) -> exp_t {
        x *= ln2<exp_t>;
        constexpr exp_t c1 = exp_t(1) / 3;
        constexpr exp_t c2 = exp_t(1) / 5;
        return 1 + x * (1 + (x >> 1) * (1 + x * c1 * (1 + (x >> 2) * (1 + x * c2))));
    };

    if (s > 0) {
        const exp_t x = static_cast<exp_t>(s.frac()) * multiplier;
        return (fixed_t(1) << int(s)) * static_cast<fixed_t>(approx(x));
    }
    else {
        s = -s;
        const exp_t x = static_cast<exp_t>(s.frac()) * multiplier;
        return (fixed_t(1) >> int(s)) * static_cast<fixed_t>(approx(-x));
    }
}

template<std::integral base, int fp>    
constexpr fixed<base, fp> exp(fixed<base, fp> x) {
    return exp2(x, log2e<exp_t>);
}

template<std::integral base, int fp>    
constexpr fixed<base, fp> exp10(fixed<base, fp> x) {
    return exp2(x, log2_10<exp_t>);
}

template<std::integral base, int fp>    
constexpr fixed<base, fp> pow(fixed<base, fp> x, exp_t y) {
    using fixed_t = fixed<base, fp>;

    if (x == 0)
        return 0;

    if (x > 0)
        return exp2(y * log2(x));
    else {
        if (y.frac() == 0)
            if (y & 1)
                return -exp2(y * log2(-x));
            else
                return  exp2(y * log2(-x));
        else
            return fixed_t::min();
    }
}

template<std::integral base, int fp>
constexpr fixed<base, fp> cbrt(fixed<base, fp> s) {
    using fixed_t = fixed<base, fp>;
    constexpr exp_t mul = exp_t(1) / 3;
    if (s > 0)
        return exp2(log2(s) * mul);
    else if (s < 0)
        return -exp2(log2(-s) * mul);
    else
        return 0;
}

// Trigonometry

template<std::integral base, int fp>
constexpr trig_t sin(fixed<base, fp> s) {
    static_assert(fixed<base, fp>::is_signed, "sin only supports signed fixed types!");

    if (s < 0)
        return -sin(-s);

    if (s > tau<decltype(s)>)
        s %= tau<decltype(s)>;

    trig_t x = s;

    const int sector = ((x + impl::p1) * impl::pstep).raw() >> trig_t::frac_bits;
    const bool sin_sign = x <= impl::p3 || x > impl::p7;

    x -= half_pi<trig_t> * sector;

    const trig_t out_sin = (sector & 1) ? impl::get_cos(x) : impl::get_sin(x);
    return sin_sign ? out_sin : -out_sin;
}

template<std::integral base, int fp>
constexpr trig_t cos(fixed<base, fp> s) {
    static_assert(fixed<base, fp>::is_signed, "cos only supports signed fixed types!");

    s = (s < 0) ? -s : s;

    if (s > tau<decltype(s)>)
        s %= tau<decltype(s)>;

    trig_t x = s;

    const int sector = ((x + impl::p1) * impl::pstep).raw() >> trig_t::frac_bits;
    const bool cos_sign = x <= impl::p1 || x > impl::p5;

    x -= half_pi<trig_t> * sector;

    const trig_t out_cos = (sector & 1) ? impl::get_sin(x) : impl::get_cos(x);
    return cos_sign ? out_cos : -out_cos;
}

template<std::integral base, int fp>
constexpr void sincos(fixed<base, fp> s, trig_t& out_sin, trig_t& out_cos) {
    static_assert(fixed<base, fp>::is_signed, "sincos only supports signed fixed types!");

    if (s < 0) {
        sincos(-s, out_sin, out_cos);
        out_sin = -out_sin;
        return;
    }
    
    if (s > tau<decltype(s)>)
        s %= tau<decltype(s)>;

    trig_t x = s;

    // Sectors in intervals of pi/2, with x initially offset by pi/4.
    const int sector = ((x + impl::p1) * impl::pstep).raw() >> trig_t::frac_bits;
    const bool cos_sign = x <= impl::p1 || x > impl::p5;
    const bool sin_sign = x <= impl::p3 || x > impl::p7;

    x -= half_pi<trig_t> * sector;

    // Odd sector = flip inputs
    if (sector & 1) {
        out_cos = impl::get_sin(x);
        out_sin = impl::get_cos(x);
    }
    else {
        out_cos = impl::get_cos(x);
        out_sin = impl::get_sin(x);
    }

    out_cos = cos_sign ? out_cos : -out_cos;
    out_sin = sin_sign ? out_sin : -out_sin;
}

template<std::integral base, int fp>
constexpr fixed<base, fp> tan(fixed<base, fp> s) {
    using fixed_t = fixed<base, fp>;

    trig_t sin, cos;
    sincos(s, sin, cos);

    return static_cast<fixed_t>(sin) * rcp(static_cast<fixed_t>(cos));
}

// Inverse Trigonometry

template<std::integral base, int fp>
constexpr trig_t asin(fixed<base, fp> s) {
    const trig_t x = s;

    if (x > 1)
        return trig_t::max();
    if (x > 0.5)
        return half_pi<trig_t> - (asin(impl::asin_sqrt((1 - x) >> 1)) << 1);
    if (x < 0)
        return -asin(-x);

    constexpr trig_t c1 =  0.16581943277089197797;
    constexpr trig_t c2 = -0.07245689041940490960;
    constexpr trig_t c3 =  0.18887846423966628273;
    constexpr trig_t c4 = -0.00292492507354176002;
    constexpr trig_t c5 =  1.00013379442769712035;
    const trig_t y = x * (x * (x * (x * (c1 * x + c2) + c3) + c4) + c5);
    return y;
}

template<std::integral base, int fp>
constexpr trig_t acos(fixed<base, fp> s) {
    return half_pi<trig_t> - asin(s);
}

template<std::integral base, int fp>
constexpr trig_t atan(fixed<base, fp> s) {
    if (s < 0)
        return -atan(-s);

    auto atan01 = [] (trig_t x) -> trig_t {
        if (x <= 0.375) {
            constexpr trig_t c1 = trig_t(1) / 3;
            constexpr trig_t c2 = trig_t(1) / 5;
            constexpr trig_t c3 = trig_t(1) / 7;
            constexpr trig_t c4 = trig_t(1) / 9;
            const trig_t x2 = x * x;
            return x * (1 - x2 * (c1 - x2 * (c2 - x2 * (c3 - x2 * c4))));
        }
        else {
            constexpr trig_t c1 = -0.07277922440465403597;
            constexpr trig_t c2 =  0.33012363557510843171;
            constexpr trig_t c3 = -0.52029663001374293341;
            constexpr trig_t c4 =  0.05631084241194499879;
            constexpr trig_t c5 =  0.99158187828010724285;
            constexpr trig_t c6 =  0.00045766154868510445;
            return x * (x * (x * (x * (c1 * x + c2) + c3) + c4) + c5) + c6;
        }
    };

    if (s <= 1)
        return atan01(s);
    else
        return half_pi<trig_t> - atan01(rcp_ext(s));
}

template<std::integral base, int fp>
constexpr trig_t atan2(fixed<base, fp> y, fixed<base, fp> x) {
    if (x == 0)
        return y >= 0 ? half_pi<trig_t> : -half_pi<trig_t>;

    const auto ratio = y * rcp(x);
    const int sign_y = sign(y);
    if (sign(ratio) != (sign_y * sign(x))) // Overflow
        return (sign_y >= 0) ? half_pi<trig_t> : -half_pi<trig_t>;

    const trig_t val = atan(ratio);

    if (x < 0) {
        if (y >= 0)
            return val + pi<trig_t>;
        else
            return val - pi<trig_t>;
    }
    else
        return val;
}

}