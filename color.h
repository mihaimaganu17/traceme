#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "traceme.h"
#include "interval.h"

using color = vec3;

// Utility function that writes a single pixel's color out to the standard output stream.
void write_color(std::ostream& out, const color& pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

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
