#pragma once

#include "./math.hpp"

#include <bit>
#include <iostream>

namespace fxd {

template<std::integral base, int fp>
std::ostream& operator<<(std::ostream& o, const fixed<base, fp> value) {
    o << static_cast<base>(value);

    base frac = value.frac().raw();
    int zero_fill = std::countl_zero(frac) - fp;
    for (; zero_fill >= 0; zero_fill--) o << '0';

    o << static_cast<base>(value.frac() * 10000);
}

}