#ifndef CAMERA_H
#define CAMERA_H

#include "traceme.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"

/*
 * The camera class is responsible for 2 impoartan jobs
 * 1. Construct and dispatch rays into the world
 * 2. Use the results of these rays to construct the rendered image.
 */
class camera {
    public:
        // Desired aspect ratio of the image. Using this and the width we can calculate the images
        // height, making sure the aspect ratio is preserved
        double aspect_ratio = 1;//16.0 / 9.0; // widht / height
        int image_width = 100;
        // Count of random samples to be generated per pixel for anti-aliasing effect.
        int samples_per_pixel = 10;
        // Limits the number of times a casted ray reflects on surfaces of the world, such that we
        // do not blow the stack
        unsigned int max_depth = 10;

        // Vertical view angle (field of view). This is the visual angle from edge to edge of the
        // rendered image. Since our image is not square, the fov is different horizontally and
        // vertically.
        double vfov = 90;

        // The point from which camera / eyes are looking
        point3 lookfrom = point3(0, 0, 0);
        // The point to which camera is looking at. It is essentially the same point which has `z`,
        // the last coordinate as the focal length
        point3 lookat = point3(0, 0, -1);
        // Camera-relative "up" direction or the `y` dependent coordinate
        point3 vup = point3(0, 1, 0);

        // Defocus blur parameters.
        // In order to accomplish defocus blue, we construct a disk centered at the camera center.
        // Larger radius = greater defocus blur.
        // The original camera we had, had a defocus disk of radius zero (no blur at all), as such
        // all the rays originated at the disk center (lookfrom).
        // Angle of the cone with apex at viewport center and base (defocus disk) as the camera
        // center.
        // Angle of the cone with apex at viewport center and base (defocus disk) as the camera
        // center.
        double defocus_angle = 0;
        // Distance from the camera lookfrom point to plane of perfect focus
        double focus_dist = 10;

        /* Camera Parameters */
        void render(const hittable& world) {
            initialize();
            // Rendering

            // Write the header: Magic Width Height and on the next line we have the maximum value
            // for each
            // color: 255
            // P3
            // W H
            // 255
            std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

            for (int j = 0; j < image_height; j++) {
                // Log progress
                // \r just moves to the beginning of the line. And `flush` makes sure we print the
                // `clong`
                // to the stderr handle
                std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
                for (int i = 0; i < image_width; i++) {
                    // Initialize the pixel color
                    color pixel_color(0,0,0);
                    // Cast the desired number of rays for each pixel
                    for (int s = 0; s < samples_per_pixel; s++) {
                        // Get a new random ray in the pixel's region square
                        ray r = get_ray(i, j);
                        // Add that color to our end result
                        pixel_color += ray_color(r, max_depth, world);
                    }
                    // Write the color, minding the fact that it has to be scaled.
                    write_color(std::cout, pixel_samples_scale * pixel_color);
                }
            }
            // Additional whitespaces are to make sure we cover the writing above
            std::clog << "\rDone.                            \n" << std::flush;
        }

    private:
        // Rendered image height
        int image_height;
        // In the end result, we will still have a single value for coloring that region. We will
        // get that color value by summing up the results we get from each random ray cast.
        // As such, we need to scale down each value, based on the number of pixels that we compute
        double pixel_samples_scale;
        // Camera center
        point3 center;
        // Location of pixel 0,0 (first pixel of the vieport
        point3 pixel00_loc;
        // Offset to pixel to the right
        vec3 pixel_delta_u;
        // Offset to pixel below
        vec3 pixel_delta_v;
        // Camera-frame basis vectors.
        // u -> to the right
        // v -> up
        // w -> reversed of focal length
        vec3 u, v, w;

        // Defocus disk horizontal radius
        vec3 defocus_disk_u;
        // Defocus disk vertical radius
        vec3 defocus_disk_v;

        // Configures the camera for rendering
        void initialize() {
            // Calculate the image height and make sure that it's at least 1.
            image_height = int(image_width / aspect_ratio);
            image_height = (image_height < 1) ? 1 : image_height;

            pixel_samples_scale = 1.0 / samples_per_pixel;

            // Focal length is the distance from the camera to the viewport. This is different from
            // the `z` coordinate of the object that we are viewing.
            //
            // When using a focus disk, we no longer need focal length
            //
            // auto focal_length = (lookfrom - lookat).length();
            // Set the camera origin.
            // In this coordinate sceme: Y points up, x points to the right and z point straight to
            // the viewport, orthogonally
            center = lookfrom;

            // We compute the viewports height based on the angle of the camera
            auto theta = degrees_to_radians(vfov);
            // Compute the tangent
            auto h = tan(theta / 2);
            // Calculate the viewports height


            // The viewport is a virtual rectangle in the 3D world that contains the grid of image
            // pixel locations. If pixels are spaced the same distance horizontally as they are
            // vertically, the viewport that bounds them will have the same aspect ratio as the
            // rendered image.
            // The distance between 2 adjacent pixels is called the pixel spacing and square pixels
            // is the standard.
            auto viewport_height = 2.0 * h * focus_dist;
            // Viewport widths less than one are ok since they are real valued.
            auto viewport_width = viewport_height * (double(image_width) / image_height);

            // Compute the u, v, w unit basis for the camera coordinate frame
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            // `u` and `v` are already unit vectors so we do not have to convert them. Theoretically
            // this should be -w x u, but we just inversed the terms
            v = cross(u, w);

            // We define 2 vectors to navigate the pixel grid of the viewport.
            // Horizontal navigation - left to right. Since X points to the right, the vector length
            // will be the X coordinate maximum.
            auto viewport_u = u * viewport_width;
            // Vertical navigation - top to bottom. Since Y points up, we set it to the maximum
            // value.
            // Since our Y coordinate for the image increases as we go render downwards, the
            // viewports Y decreases, since the coordinate system of the camera has Y pointing up.
            // This means the 2 Ys are inverted.
            auto viewport_v = v * viewport_height;

            // We will define the distance from pixel to pixel (of the viewport) as a delta, which
            // has to be computed for both horizontal and vertical directions.
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            // Calculate the location of the upper-left pixel
            // Our pixel grid will be inset from the viewport edges by half the pixel-to-pixel
            // distance.
            // This way, our viewport area is evenly divided into width × height identical regions.
            // Because the camera origin is theoretically orthogonal to the center of the viewport,
            // the most upper left region of the viewport is found by substracting half of every
            // distance.
            // In the schema, this is Q of P(t) = Q + t * d
            auto viewport_upper_left = center
                - (focus_dist * w) - viewport_u/2 - viewport_v/2;
            // The pixel grid is inset by the viewport edges by half-pixel, such that each pixel
            // has the same space region around it. That is why, in order to find the first pixel,
            // we have to move by half pixel from the viewports upper left region (Q).
            pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

            // Calculate the camera defocus disk basis vectors
            // Not sure about this formula
            // https://en.wikipedia.org/wiki/Circle_of_confusion
            auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
            defocus_disk_u = u * defocus_radius;
            defocus_disk_v = v * defocus_radius;
        }

        // Returns the color for a given scene ray.
        // This function will linearly blend white and blue depending on the height of the
        // 𝑦 coordinate
        // after scaling the ray direction to unit length (so −1.0<𝑦<1.0).
        // Because we're looking at the 𝑦 height after normalizing the vector, you'll notice a
        // horizontal
        // gradient to the color in addition to the vertical gradient.

        // When 𝑎=1.0, we want blue. When 𝑎=0.0, we want white. In between, we want a blend.
        // This forms a “linear blend”, or “linear interpolation”.
        // This is commonly referred to as a lerp between two values.
        color ray_color(const ray& r, unsigned int depth, const hittable& world) const {
            // Check if we still want to reflect
            if (depth <= 0) {
                return color(0, 0, 0);
            }

            hit_record hit;

            // Compute the points where we hit objects from the world, if any
            //
            // Material science
            // If a ray bounces off of a material and keeps 100% of its color, then we say that the
            // material is white. If a ray bounces off of a material and keeps 0% of its color,
            // then we say that the material is black.
            //
            // Fixing shadow Acne
            //
            // Start of the interval is 0.001, because a ray intersection with a surface is
            // susceptible to floating point rounding errors, meaning the intersection point might
            // not always be on the surface. If it is below the surface, there is a high change
            // that it will intersect that surface again
            if (world.hit(r, interval(0.001, infinity), hit) == true) {
                // Prepare parameters for a reflected ray from the surface that is goind to be hit
                // by our ray casts
                ray scattered;
                color attenuation;

                // If we hit and reflect
                if (hit.mat->scatter(r, hit, attenuation, scattered)) {
                    // We cast the reflected ray recursively
                    return attenuation * ray_color(scattered, depth - 1, world);
                }
                // Otherwise, we just return black
                return color(0, 0, 0);

                // Rest of these comments apply when we do not have materials
                /* 
                // We get a random new reflection direction, using rejection method.
                // vec3 reflect_direction = random_on_hemisphere(hit.normal);
                // Create a ray relfection direction from the Lambertian distribution. This can
                // be easily understood with the vector addition triangle rule and the explanation
                // from 9.4 True Lambertian Reflection from:
                // https://raytracing.github.io/books/RayTracingInOneWeekend.html#overview
                //vec3 reflect_direction = hit.normal + random_unit_vector();
                // Now we map each component to interval from 0 to 1 and at the same time, we map
                // it to (r,
                // g, b). We add plus 1 to ensure that we cover the entire [0, 1] interval
                // As a first demo of our new diffuse materail we'll set the ray_color function to
                // return 50% of the color from a bounce.
                // We construct a new ray that reflects back with the above direction
                //ray reflect_ray(hit.p, reflect_direction);
                // Now we recursively reflect that in the world
                //return 0.5 * ray_color(reflect_ray, depth - 1, world);
                // Alternatively, we can write hit.normal + color(1,1,1)
                */
            }
            // Get the unit vector from out ray.
            vec3 unit_direction = unit_vector(r.direction());
            // We are blending linearly, based on the y height (top to bottom). So we compute a as
            // the
            // pixel.
            auto a = 0.5*(unit_direction.y() + 1.0);
            // We want to return a blend between blue and white.
            color white(1.0, 1.0, 1.0);
            // 170, 199, 250
            color light_blue(0.5, 0.7, 1.0);
            // blendedValue = (1 - a) * startValue + a * endValue.
            // (1.0 - a) tells that, while we go down in the image rendering in the viewport
            // (y decreseases)
            // we have more white, and less blue.
            auto c = ((1.0-a) * white + a * light_blue);
            return c;
        }

        // Construct a camera ray cast from the origin and directed at a randomly sampled point
        // in the square region that has pixel (i,j) as the center.
        ray get_ray(int i, int j) const {
            // Construct a camera ray originating from the defocus disk and directed at a randomly
            // samples point around the pixel location i, j.
            // Get the random offset
            auto rand_offset= sample_square();

            // Go to that pixel point in the viewport pixel grid, which is relative to the current
            // pixel
            auto rand_pixel = pixel00_loc
                + ((i + rand_offset.x()) * pixel_delta_u)
                + ((j + rand_offset.y()) * pixel_delta_v);

            // Specify the ray's origin, which is the camera center
            //auto ray_origin = center;
            // However we are now using a focus disk, so we choose a random point from the focus
            // disk to be the origin of the ray.
            auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
            // Ray direction is towards the above random pixel
            auto ray_direction = rand_pixel - ray_origin;

            // Return the new ray
            return ray(ray_origin, ray_direction);
        }

        // Function that samples a point within the region of a pixel's square. Given a grid
        // of squares, every 2 pixels are separated by a 1.0 distance. The entire grid is also
        // spaced out from the viewport edge by an 0.5 distance.
        // This means that around each pixel, there is a square surface and the orthogonal distance
        // from the pixel itself to each of the hypothetical square's edge is 0.5.
        // In the viewport below, we considered each vertical bar = 2 horizontal dashes, so it
        // makes sens to the eye.
        // +-----+-----------------+
        // |  |  |  |  |  |  |  |  |
        // |--p--|--p--|--p--|--p--|
        // |  |  |  |  |  |  |  |  |
        // |--+--+--+--+--+--+--+--+
        // |  |  |  |  |  |  |  |  |
        // |--p--+--p--+--p--+--p--|
        // |  |  |  |  |  |  |  |  |
        // +-----------------------+
        vec3 sample_square() const {
            // Return a vector sampled in an unit square, ignoring the z coordinate, which is
            // non-existent in a 2D plane
            return vec3(random_double() - 0.5, random_double() - 0.5, 0);
        }

        // Returns a random point in the camera defocus disk.
        point3 defocus_disk_sample() const {
            // Get a random unit disk point
            auto p = random_in_unit_disk();

            // Get the pixel on the disk for that point. 3 simple rule here. One unit in the unit
            // disk corresponds to the entire disk length.
            return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
        }
};

#endif
