#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "traceme.h"
#include "interval.h"

using color = vec3;

// Transforms our data into gamma space so that our image viewer can more accurately display our
// image. As a simple approximation, we can use “gamma 2” as our transform, which is the power that
// you use when going from gamma space to linear space. We need to go from linear space to gamma
// space, which means taking the inverse of “gamma 2", which means an exponent of 1/gamma, which
// is just the square-root. We'll also want to ensure that we robustly handle negative inputs.
inline double linear_to_gamma(double linear_component) {
    if (linear_component > 0)
        return sqrt(linear_component);
    return 0;
}

// Utility function that writes a single pixel's color out to the standard output stream.
void write_color(std::ostream& out, const color& pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Apply a linear to gamma transform for gamma 2
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // Translate the [0, 1] component values to the byte range [0,255].
    static const interval intensity(0.000, 0.999);

    // What we do here is basically trying to blend out the values
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));

    // Write out the pixel color components
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif
