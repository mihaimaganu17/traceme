// Include our own defined headers
#include "traceme.h"
#include "camera.h"
#include "sphere.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"


int main() {
    // World / Scene configuration
    hittable_list world;

    //world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    //world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    // We define some material
    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
    auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    // We create a bunch of spheres
    auto ground_sphere = make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground);
    // Center sphere that is slightly closer to the camera
    auto center_sphere = make_shared<sphere>(point3(0.0, 0.0, -1.2), 0.5, material_center);
    // A sphere we put to the left
    auto left_sphere = make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left);
    // A sphere we put to the right
    auto right_sphere = make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right);

    world.add(ground_sphere);
    world.add(center_sphere);
    world.add(left_sphere);
    world.add(right_sphere);


    // Set up the camera through which we view the world
    camera cam;

    // Change the aspect ratio to something more popular
    cam.aspect_ratio = 16.0 / 9.0;
    // Change image's width. This will automatically also change the images height as well.
    cam.image_width = 400;
    // Set the number of ray samples we want to cast for each pixel to do anti-aliasing
    cam.samples_per_pixel = 100;
    // Set the number of times we want the casted rays to reflect on surfaces of the world
    cam.max_depth = 50;

    cam.render(world);
}
