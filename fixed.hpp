#pragma once

#include <climits>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace fxd {
	using i8  = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;
	using i64 = int64_t;

	using u8  = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;

namespace impl {
    template<std::integral T>
    struct next_int;

    template<>
    struct next_int<u8> {
        using type = u16;
    };

    template<>
    struct next_int<u16> {
        using type = u32;
    };

    template<>
    struct next_int<u32> {
        using type = u64;
    };

    template<>
    struct next_int<u64>;

    template<>
    struct next_int<i8> {
        using type = i16;
    };

    template<>
    struct next_int<i16> {
        using type = i32;
    };

    template<>
    struct next_int<i32> {
        using type = i64;
    };

    template<>
    struct next_int<i64>;

    template<std::integral T>
    using next_int_v = next_int<T>::type;
}

template<std::integral base, int fp> 
class fixed {
    static constexpr base one = base(1);
    using next_t = impl::next_int_v<base>;
    base _data;
public:
    using fixed_t = fixed<base, fp>;
    using base_type = base;
    static constexpr bool is_signed = std::is_signed_v<base>;
    static constexpr int bits = sizeof(base) * CHAR_BIT;
    static constexpr int frac_bits = fp;
    static constexpr int int_bits = bits - frac_bits - is_signed;

    static_assert(frac_bits >= 4, "Must have at least four fraction bits!");
    static_assert(frac_bits < (bits - is_signed), "Fraction bits cannot exceed integer width!");

    static constexpr base max_int = (one << int_bits) - 1;
    static constexpr base frac_mask = (one << fp) - 1;
    static constexpr base int_mask = static_cast<base>( (~frac_mask) ^ (is_signed << (bits - 1)) );

    template<std::integral T>
    static constexpr fixed_t from_raw(T b) {
        fixed_t out;
        out._data = static_cast<base>(b);
        return out;
    }

    static constexpr fixed_t max_frac() { 
        return from_raw(frac_mask); 
    };

    static constexpr fixed_t min_frac() { 
        return from_raw(1); 
    };

    static constexpr fixed_t max() {
        return from_raw(std::numeric_limits<base>::max());
    };

    static constexpr fixed_t min() {
        return from_raw(std::numeric_limits<base>::min() + is_signed);
    };

    constexpr fixed() = default;
    constexpr fixed(const fixed_t&) = default;
    constexpr fixed(fixed_t&&) = default;
    constexpr fixed& operator=(const fixed_t&) = default;
    constexpr fixed& operator=(fixed_t&&) = default;

    constexpr fixed(base value) {
        _data = value << fp;
    }

    constexpr base raw() const {
        return _data;
    }

    template<std::floating_point T>
    constexpr fixed(T value) {
        _data = static_cast<base>(value * T(one << fp));
    }
    
    template<std::integral other_base, int other_fp>
    constexpr fixed(fixed<other_base, other_fp> other) {
        *this = other.operator fixed<base, fp>();
    }

    constexpr fixed frac() const { 
        if constexpr(is_signed) {
            const base d = (_data < 0) ? (-_data) : _data;
            return from_raw(d & frac_mask);
        }
        else {
            return from_raw(_data & frac_mask);
        }
    }

    template<std::floating_point T>
    constexpr explicit operator T() const {
        return static_cast<T>(_data) / T(one << fp);
    }

    template<std::integral T>
    constexpr explicit operator T() const {
        if constexpr(is_signed)
            return static_cast<T>(_data / (one << fp));
        else
            return static_cast<T>(_data >> fp);
    }

    template<std::integral other_base, int other_fp>
    constexpr explicit operator fixed<other_base, other_fp>() const {
        using other_t = fixed<other_base, other_fp>;

        if constexpr(other_fp > fp) {
            next_t data = static_cast<next_t>(_data);
            return other_t::from_raw(data << (other_fp - fp));
        }
        else if constexpr(other_fp < fp) {
            if constexpr(other_t::is_signed) {
                return other_t::from_raw(_data / (one << (fp - other_fp)));
            }
            else {
                return other_t::from_raw(_data >> (fp - other_fp));
            }
        }
        else {
            return other_t::from_raw(_data);
        }
    }

    constexpr fixed_t operator-() const { return from_raw(-_data); }
    constexpr fixed_t operator~() const { return from_raw(~_data); }

    constexpr fixed_t& operator++()    { _data += one << fp; return *this; }
    constexpr fixed_t& operator++(int) { _data += one << fp; return *this; }
    constexpr fixed_t& operator--()    { _data -= one << fp; return *this; }
    constexpr fixed_t& operator--(int) { _data -= one << fp; return *this; }

    constexpr fixed_t& operator+=(const fixed_t other) {
        _data += other._data;
        return *this;
    }

    constexpr fixed_t& operator-=(const fixed_t other) {
        _data -= other._data;
        return *this;
    }

    constexpr fixed_t& operator*=(const fixed_t other) {
        const next_t mul = static_cast<next_t>(_data) * static_cast<next_t>(other._data);
        _data = static_cast<base>(mul >> fp);
        return *this;
    }

    constexpr fixed_t& operator/=(const fixed_t other) {
        const next_t div = (static_cast<next_t>(_data) << fp) / static_cast<next_t>(other._data);
        _data = static_cast<base>(div);
        return *this;
    }

    constexpr fixed_t& operator%=(const fixed_t other) {
        const next_t rem = static_cast<next_t>(_data) % static_cast<next_t>(other._data);
        _data = static_cast<base>(rem);
        return *this;
    }

    constexpr fixed_t& operator&=(const base other) {
        _data &= other;
        return *this;
    }

    constexpr fixed_t& operator|=(const base other) {
        _data |= other;
        return *this;
    }

    constexpr fixed_t& operator^=(const base other) {
        _data ^= other;
        return *this;
    }

    constexpr fixed_t& operator<<=(const int other) {
        _data <<= other;
        return *this;
    }

    constexpr fixed_t& operator>>=(const int other) {
        _data >>= other;
        return *this;
    }

    constexpr friend fixed_t operator+(const fixed_t a, const fixed_t b) {
        return from_raw(a._data + b._data);
    }

    constexpr friend fixed_t operator-(const fixed_t a, const fixed_t b) {
        return from_raw(a._data - b._data);
    }

    constexpr friend fixed_t operator*(const fixed_t a, const fixed_t b) {
        const next_t mul = static_cast<next_t>(a._data) * static_cast<next_t>(b._data);
        return from_raw(static_cast<base>(mul >> fp));
    }

    constexpr friend fixed_t operator/(const fixed_t a, const fixed_t b) {
        const next_t div = (static_cast<next_t>(a._data) << fp) / static_cast<next_t>(b._data);
        return from_raw(static_cast<base>(div));
    }

    constexpr friend fixed_t operator%(const fixed_t a, const fixed_t b) {
        const next_t rem = static_cast<next_t>(a._data) % static_cast<next_t>(b._data);
        return from_raw(static_cast<base>(rem));
    }

    constexpr friend fixed_t operator&(const fixed_t a, const base b) {
        return from_raw(a._data & b);
    }

    constexpr friend fixed_t operator|(const fixed_t a, const base b) {
        return from_raw(a._data | b);
    }

    constexpr friend fixed_t operator^(const fixed_t a, const base b) {
        return from_raw(a._data ^ b);
    }

    constexpr friend fixed_t operator>>(const fixed_t a, const int b) {
        return from_raw(a._data >> b);
    }

    constexpr friend fixed_t operator<<(const fixed_t a, const int b) {
        return from_raw(a._data << b);
    }

    constexpr friend bool operator>(const fixed_t a, const fixed_t b) {
        return a._data > b._data;
    }

    constexpr friend bool operator>=(const fixed_t a, const fixed_t b) {
        return a._data >= b._data;
    }

    constexpr friend bool operator<(const fixed_t a, const fixed_t b) {
        return a._data < b._data;
    }

    constexpr friend bool operator<=(const fixed_t a, const fixed_t b) {
        return a._data <= b._data;
    }

    constexpr friend bool operator==(const fixed_t a, const fixed_t b) {
        return a._data == b._data;
    }
};

using hfixed12  = fixed<i16,  12>; // 1 sign bit, 3 integer bits, 12 fraction bits
using hfixed8   = fixed<i16,  8>;  // 1 sign bit, 7 integer bits, 8  fraction bits

using uhfixed12 = fixed<u16, 12>; // 4 integer bits, 12 fraction bits
using uhfixed8  = fixed<u16, 8>;  // 8 integer bits, 8  fraction bits

using fixed8    = fixed<i32,  8>;   // 1 sign bit, 23 integer bits, 8  fraction bits
using fixed12   = fixed<i32,  12>;  // 1 sign bit, 19 integer bits, 12 fraction bits
using fixed16   = fixed<i32,  16>;  // 1 sign bit, 15 integer bits, 16 fraction bits
using fixed18   = fixed<i32,  18>;  // 1 sign bit, 13 integer bits, 18 fraction bits
using fixed20   = fixed<i32,  20>;  // 1 sign bit, 11 integer bits, 20 fraction bits
using fixed24   = fixed<i32,  24>;  // 1 sign bit, 7 integer bits,  24 fraction bits

using ufixed8   = fixed<u32, 8>;  // 24 integer bits, 8  fraction bits
using ufixed12  = fixed<u32, 12>; // 20 integer bits, 12 fraction bits
using ufixed16  = fixed<u32, 16>; // 16 integer bits, 16 fraction bits
using ufixed18  = fixed<u32, 18>; // 14 integer bits, 18 fraction bits
using ufixed20  = fixed<u32, 20>; // 12 integer bits, 20 fraction bits
using ufixed24  = fixed<u32, 24>; // 8  integer bits, 24 fraction bits

using exp_t     = fixed<i32, 26>; // 1 sign bit, 5 integer bits, 26 fraction bits
using high_t    = fixed<i32, 26>; // 1 sign bit, 5 integer bits, 26 fraction bits
using trig_t    = fixed<i32, 27>; // 1 sign bit, 4 integer bits, 27 fraction bits
using frac_t    = fixed<i32, 28>; // 1 sign bit, 3 integer bits, 28 fraction bits

}

namespace std {

template<std::integral base, int fp>
struct hash<fxd::fixed<base, fp>> {
    std::size_t operator()(const fxd::fixed<base, fp>& x) const {
        return std::hash<base>()(x.raw());
    }
};

}