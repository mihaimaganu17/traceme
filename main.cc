// Include our own defined headers
#include "vec3.h"
#include "ray.h"
#include "color.h"

#include <iostream>

// Computes whether or not, a casted ray 'r' intersect the sphere defined by a `center` point and
// a `radius` and returns the `t` where the ray intersects the sphere for the first time.
double hit_sphere(const point3& center, double radius, const ray& r) {
    // We need to solve a*x^2 + b*x + c = 0
    // Vector between the center of the sphere and the origin of the ray cast
    auto ray_to_sphere_center = center - r.origin();
    auto a = dot(r.direction(), r.direction());
    // We actually consider replacing b with (-2h), which gives
    auto h = dot(r.direction(), ray_to_sphere_center);
    auto c = dot(ray_to_sphere_center, ray_to_sphere_center) - radius * radius;

    // We need to compute the term for the square root and check it's sign
    double discriminant = h - a * c;

    // If the term is negative, we do not have any real solution so the ray never intersect the
    // sphere
    if (discriminant < 0) {
        return -1.0;
    // If the term is zero or positive , there is at least one solution and thus the ray cast
    // intersects the sphere one or 2 points. Currently, we only care about the first hit, which
    // we can see, and thus we only return the - sqrt solution
    } else {
        return ((h - sqrt(discriminant))/ * a);
    }
}

// Returns the color for a given scene ray.
// This function will linearly blend white and blue depending on the height of the 𝑦 coordinate
// after scaling the ray direction to unit length (so −1.0<𝑦<1.0).
// Because we're looking at the 𝑦 height after normalizing the vector, you'll notice a horizontal
// gradient to the color in addition to the vertical gradient.

// When 𝑎=1.0, we want blue. When 𝑎=0.0, we want white. In between, we want a blend.
// This forms a “linear blend”, or “linear interpolation”.
// This is commonly referred to as a lerp between two values.
color ray_color(const ray& r) {
    // Place the center of the sphere on the `-1` of the z-axis. Essentially meaning this is at
    // distance 1 orthogonal in front of the camera. Setting z to a higher value, would shrink the
    // sphere, as it would further.
    point3 sphere_center(0, 0, -1);
    // Git the sphere a radius
    double sphere_radius(0.5);
    // Compute the first point where we hit the sphere, if any
    double t = hit_sphere(sphere_center, sphere_radius, r);

    // If t is negative, we did not hit the sphere, so we ignore that
    if (t > 0.0) {
        // Compute the normal
        // First we compute the ray vector
        vec3 ray_vec = r.at(t);
        // Then we compute the normal (vec pependicular to the hit point) and normalize it, using
        // the radius of the sphere.
        vec3 normal = unit_vector(ray_vec - sphere_center);
        // Now we map each component to interval from 0 to 1 and at the same time, we map it to (r,
        // g, b). We add plus 1 to ensure that we cover the entire [0, 1] interval
        return 0.5*color(normal.x() + 1, normal.y() + 1, normal.z() + 1);
    }
    // Get the unit vector from out ray.
    vec3 unit_direction = unit_vector(r.direction());
    // We are blending linearly, based on the y height (top to bottom). So we compute a as the
    // pixel.
    auto a = 0.5*(unit_direction.y() + 1.0);
    // We want to return a blend between blue and white.
    color white(1.0, 1.0, 1.0);
    // 170, 199, 250
    color light_blue(0.5, 0.7, 1.0);
    // blendedValue = (1 - a) * startValue + a * endValue.
    // (1.0 - a) tells that, while we go down in the image rendering in the viewport (y decreseases)
    // we have more white, and less blue.
    return ((1.0-a) * white + a * light_blue);
}

int main() {
    // Desired aspect ratio of the image. Using this and the width we can calculate the images
    // height, making sure the aspect ratio is preserved
    auto aspect_ratio = 16.0 / 9.0; // widht / height
    int image_width = 400;

    // Calculate the image height and make sure that it's at least 1.
    int image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    // Camera
    // Focal length is the distance from the camera to the viewport. This is different from the
    // `z` coordinate of the object that we are viewing.
    auto focal_length = 1.0;
    // Set the camera origin.
    // In this coordinate sceme: Y points up, x points to the right and z point straight to the
    // viewport, orthogonally
    auto camera_center = point3(0,0,0);

    // The viewport is a virtual rectangle in the 3D world that contains the grid of image pixel
    // locations. If pixels are spaced the same distance horizontally as they are vertically, the
    // viewport that bounds them will have the same aspect ratio as the rendered image.
    // The distance between 2 adjacent pixels is called the pixel spacing and square pixels is the
    // standard.
    auto viewport_height = 2.0;
    // Viewport widths less than one are ok since they are real valued.
    auto viewport_width = viewport_height * (double(image_width) / image_height);

    // We define 2 vectors to navigate the pixel grid of the viewport.
    // Horizontal navigation - left to right. Since X points to the right, we set it to maximum.
    auto viewport_u = vec3(viewport_width, 0, 0);
    // Vertical navigation - top to bottom. Since Y points up, we set it to the maximum value.
    // Since our Y coordinate for the image increases as we go render downwards, the viewports Y
    // decreases, since the coordinate system of the camera has Y pointing up. This means the 2 Ys
    // are inverted.
    auto viewport_v = vec3(0, -viewport_height, 0);

    // We will define the distance from pixel to pixel (of the viewport) as a delta, which has to
    // be computed both horizontally and vertically.
    auto pixel_delta_u = viewport_u / image_width;
    auto pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper-left pixel
    // Our pixel grid will be inset from the viewport edges by half the pixel-to-pixel distance.
    // This way, our viewport area is evenly divided into width × height identical regions.
    // Because the camera origin is theoretically orthogonal to the center of the viewport, the
    // most upper left region of the viewport is found by substracting half of every distance.
    // In the schema, this is Q of P(t) = Q + t * d
    auto viewport_upper_left = camera_center - vec3(0,0,focal_length) - viewport_u/2 - viewport_v/2;
    // The pixel grid is inset by the viewport edges by half-pixel, such that each pixel has the
    // same space region around it. That is why, in order to find the first pixel, we have to move
    // by half pixel from the viewports upper left region (Q).
    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Rendering

    // Write the header: Magic Width Height and on the next line we have the maximu value for each
    // color: 255
    // P3
    // W H
    // 255
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {
        // Log progress
        // \r just moves to the beginning of the line. And `flush` makes sure we print the `clong`
        // to the stderr handle
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; i++) {
            // Compute the pixel center of the current pixel. This is the done by moving the 2
            // viewport deltas by the amount of the current image reion.
            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            // Compute the ray direction going from the camera to the pixel.
            auto ray_direction = pixel_center - camera_center;
            // Initialize the ray
            ray r(camera_center, ray_direction);
            // Contruct the color for the pixel;
            color pixel_color = ray_color(r);

            write_color(std::cout, pixel_color);
        }
    }
    // Additional whitespaces are to make sure we cover the writing above
    std::clog << "\rDone.                            \n" << std::flush;
}
