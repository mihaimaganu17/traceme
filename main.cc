// Include our own defined headers
#include "vec3.h"
#include "color.h"

#include <iostream>

int main() {
    // Image configuration
    int image_width = 256;
    int image_height = 256;

    // Rendering

    // Write the header: Magic Width Height and on the next line we have the maximu value for each
    // color: 255
    // P3
    // W H
    // 255
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {
        // Log progress
        // \r just moves to the beginning of the line. And `flush` makes sure we print the `clong`
        // to the stderr handle
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; i++) {
            // red
            auto r = double(i) / (image_width - 1);
            // green
            auto g = double(j) / (image_height - 1);
            // blue
            auto b = 0.0;

            auto pixel_color = color(r, g, b);
            write_color(std::cout, pixel_color);
        }
    }
    // Additional whitespaces are to make sure we cover the writing above
    std::clog << "\rDone.                            \n" << std::flush;
}
