#ifndef TRACEME_H
#define TRACEME_H

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>

// C++ std functions we use often
using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

// Converts an angle from degress to radians
inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

// Traceme own defined headers
#include "color.h"
#include "vec3.h"
#include "ray.h"
#include "interval.h"

#endif
