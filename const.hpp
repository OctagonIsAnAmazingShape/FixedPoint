#pragma once

namespace fxd {

template<typename T>
concept fixed_point = requires {
    typename T::fixed_t;
    typename T::base_type;
    T::frac_bits;
    T::int_bits;
};

template<fixed_point T>
constexpr T pi      = T(3.1415926535897932385);

template<fixed_point T>
constexpr T sqrt_pi = T(1.7724538509055160273);

template<fixed_point T>
constexpr T half_pi = T(1.5707963267948966192);

template<fixed_point T>
constexpr T tau     = T(6.2831853071795864769);

template<fixed_point T>
constexpr T e       = T(2.7182818284590452354);

template<fixed_point T>
constexpr T e2      = T(7.3890560989306502272);

template<fixed_point T>
constexpr T phi     = T(1.6180339887498948482);

template<fixed_point T>
constexpr T sqrt_2  = T(1.4142135623730950488);

template<fixed_point T>
constexpr T rsqrt_2 = T(0.7071067811865475244);

template<fixed_point T>
constexpr T sqrt_3  = T(1.7320508075688772935);

template<fixed_point T>
constexpr T ln2     = T(0.6931471805599453094);

template<fixed_point T>
constexpr T ln10    = T(2.3025850929940456840);

template<fixed_point T>
constexpr T log2e   = T(1.4426950408889634074);

template<fixed_point T>
constexpr T log2_10 = T(3.3219280948873623479);

template<fixed_point T>
constexpr T log10_2 = T(0.3010299956639811952);

}