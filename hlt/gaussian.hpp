#pragma once

#include "hlt/location.hpp"
#include <cmath>
#include <utility>
using std::pair;

using namespace hlt;

#define SQR(x) ((x)*(x))

// Assumes is centered

namespace gaussian {

    // Calculated exp(-g * |x|^2 )
    double value(double g, Location x) {
        return exp(-g * (SQR(x.pos_x) + SQR(x.pos_y)));
    }

    // Calculate the gradient
    // Calculated exp(-g * |x|^2 ) * (-g) * 2x
    Location gradient(double g, Location x) {
        return x * (value(g, x) * g * -2);
    }

    pair<double, Location> value_and_gradient(double g, Location x) {
        double val = value(g, x);
        return std::make_pair(val, x * (val * g * -2));
    }

}
