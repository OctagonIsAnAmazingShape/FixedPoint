#include <cmath>
#include <format>
#include <iomanip>
#include <iostream>

#include "fixed.hpp"
#include "math.hpp"

int main() {
    using real = double;

    [[maybe_unused]] real start = 0;
    [[maybe_unused]] real end = 2048.0;
    [[maybe_unused]] real max_error = -999;
    [[maybe_unused]] real total_error = 0;
    [[maybe_unused]] real count = 0;

    std::cout << std::fixed << std::setprecision(15);

    fxd::fixed20 f = 1536.25724;
    std::cout << int(f) << '.' << int(f.frac() * 1000) << '\n';

    /*for (real i = -end; i < end; i += M_PI/32) {
        fxd::trig_t s, c;
        fxd::sincos(fxd::trig_t(i), s, c);

        real ds, dc;
        sincos(i, &ds, &dc);

        if (std::abs(ds) <= 1e-05 || std::abs(dc) <= 1e-05)
            continue;

        auto outs = std::format("sin i: {: .4f}, fixed: {: .10f}, real: {: .10f}, rerror: {: .6f}%\n",
            i, (double)s, ds, 100.0 * std::abs(((double)s - ds)/ds));
        auto outc = std::format("cos i: {: .4f}, fixed: {: .10f}, real: {: .10f}, rerror: {: .6f}%\n",
            i, (double)c, dc, 100.0 * std::abs(((double)c - dc)/dc));
        std::cout << outc << outs;
    }*/

    /*for (real i = start; i < end; i += M_PI/32) {
        fxd::trig_t s, c;
        fxd::sincos(fxd::fixed20(i), s, c);
        std::cout << double(s * s + c * c) << '\n';
    }*/

    for (real i = 1; i < 2.0; i += 0.001) {
        if (i == 1) continue;
        real result = (real)fxd::log2<int, 28>(i);
        real expect = std::log2(i);
        real rerror = (result - expect) / expect;
        auto out = std::format("i: {: .4f}, fixed: {: .10f}, real: {: .10f}, rerror: {: .6f}%\n",
            i, result, expect, 100.0 * rerror);
        std::cout << out;
        if (std::abs(rerror) > max_error)
            max_error = std::abs(rerror);
        if (!std::isnan(rerror)) total_error += rerror;
        count++;
    }
    std::cout << max_error * 100 << "% max rerror\n" << (total_error / count) * 100 << "% average\n";

    /*for (double i = 0; i < 1.0; i += 0.0000001) {
        real result = (real)fxd::asin(fxd::trig_t(i));
        real expect = std::asin(i);
        real rerror = (result - expect) / expect;
        if (std::abs(rerror) > max_error)
            max_error = std::abs(rerror);
        if (!std::isnan(rerror)) total_error += rerror;
        count++;
    }
    std::cout << max_error * 100 << "% max rerror\n" << (total_error / count) * 100 << "% average\n";*/

    /*for (real i = start; i < end; i += 12.168743971) {
        real result = (real)fxd::cbrt<int, 20>(i);
        real expect = std::cbrt(i);
        real rerror = (result - expect) / expect;
        auto out = std::format("i: {: .4f}, fixed: {: .10f}, real: {: .10f}, rerror: {: .6f}%\n",
            i, result, expect, 100.0 * rerror);
        std::cout << out;

        if (std::abs(rerror) > max_error)
            max_error = std::abs(rerror);
        total_error += rerror; count++;
    }
    std::cout << max_error * 100 << "% max rerror\n" << (total_error / count) * 100 << "% average\n";*/

    /*for (real i = start; i < end; i += 2.78178671) {
        real result = (real)fxd::tan<int, 20>(i);
        real expect = std::tan(i);
        real rerror = (result - expect) / expect;
        auto out = std::format("i: {: .4f}, fixed: {: .10f}, real: {: .10f}, rerror: {: .6f}%\n",
            i, result, expect, 100.0 * rerror);
        std::cout << out;

        if (std::abs(rerror) > max_error)
            max_error = std::abs(rerror);
    }
    std::cout << max_error * 100 << "% max rerror\n";*/

    /*for (real i = 0.0; i < 6.28; i += pi / 128) {
        real d_sin = std::sin(i);
        real d_cos = std::cos(i);
        real d_atan2 = std::atan2(d_sin, d_cos);

        fxd::trig_t v = i;
        fxd::fixed24 f_sin = fxd::sin(v);
        fxd::fixed24 f_cos = fxd::cos(v);
        fxd::trig_t f_atan2 = fxd::atan2(f_sin, f_cos);

        real result = real(f_atan2);
        real expect = d_atan2;

        real rerror = (result - expect) / expect;

        auto out = std::format("y: {: .10f}, x: {: .10f} - ", real(f_sin), real(f_cos));
        std::cout << out;

        out = std::format("i: {: .4f}, fixed: {: .10f}, real: {: .10f}, rerror: {: .6f}%\n",
            i, result, expect, 100.0 * rerror);
        std::cout << out;

        if (std::abs(rerror) > max_error)
            max_error = std::abs(rerror);
        if (!std::isnan(rerror)) total_error += rerror; count++;
    }
    std::cout << max_error * 100 << "% max rerror\n" << (total_error / count) * 100 << "% average\n";*/

    /*for (real i = start; i < end;) {
        real result = (real)fxd::atan<int, 20>(i);
        real expect = std::atan(i);
        real rerror = (result - expect) / expect;
        auto out = std::format("i: {: .4f}, fixed: {: .10f}, real: {: .10f}, rerror: {: .6f}%\n",
            i, result, expect, 100.0 * rerror);
        std::cout << out;

        if (std::abs(rerror) > max_error)
            max_error = std::abs(rerror);

        if (i < 1)
            i += 1.0 / (1 << 7);
        else
            i += 33.268728988253;
    }
    std::cout << max_error * 100 << "% max rerror\n";*/

    /*for (real i = -M_PI * 2; i <= M_PI * 2; i += M_PI/16) {
        real result = (real)fxd::cos<int, 24>(i);
        real expect = std::cos(i);
        real rerror = (result - expect) / expect;
        auto out = std::format("i: {: .4f}, fixed: {: .10f}, real: {: .10f}, rerror: {: .6f}%\n",
            i, result, expect, 100.0 * rerror);
        std::cout << out;

        if (std::abs(rerror) > max_error)
            max_error = std::abs(rerror);
    }
    std::cout << max_error * 100 << "% max rerror\n";*/

    /*for (real i = start; i < end; i += 14.31257891) {
        real result = (real)fxd::rcp<int, 20>(i);
        real expect = 1.0 / i;
        auto out = std::format("i: {: .4f}, fixed: {: .10f}, real: {: .10f}, rerror: {: .6f}%\n",
            i, result, expect, 100.0 * std::abs((result - expect)/expect));
        std::cout << out;
    }*/

    /*for (real i = start; i < end; i += 17.157891356) {
        real result = (real)fxd::sqrt(fxd::fixed20(i));
        real expect = sqrt(i);
        auto out = std::format("i: {: .4f}, fixed: {: .10f}, real: {: .10f}, rerror: {: .6f}%\n",
            i, result, expect, 100.0 * std::abs((result - expect)/expect));
        std::cout << out;
    }*/

    /*for (real i = start; i < end; i += 17.157891356) {
        real result = (real)fxd::rsqrt(fxd::fixed20(i));
        real expect = 1.0 / sqrt(i);
        auto out = std::format("i: {: .4f}, fixed: {: .10f}, real: {: .10f}, rerror: {: .6f}%\n",
            i, result, expect, 100.0 * std::abs((result - expect)/expect));
        std::cout << out;
    }*/
}