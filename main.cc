// Include our own defined headers
#include "traceme.h"
#include "camera.h"
#include "sphere.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"


void world_with_spheres(hittable_list& world) {
    //world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    //world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    // We define some material
    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    //auto material_left = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
    //auto material_left = make_shared<dielectric>(1.50);
    // Air divided by water refractive index
    //auto material_left = make_shared<dielectric>(1.00 / 1.33);
    // We want to have a sphere inside another sphere.
    // The outer sphere is a standard glass sphere with a refractive index of around 1.5.
    auto material_outer_sphere = make_shared<dielectric>(1.50);
    // The inner sphere's refractive index should be relative to the material of the surrounding
    // outer sphere, thus modeling a transition from glass into the inner air.
    // This translates / can be specified as the ratio of the refractive index of the
    // (contained) object divided by the refractive index of the enclosing medium (outer object).
    // The inner sphere would have a refractive index of air (the inner sphere material) over the
    // index of refraction of glass (1.0 / 1.5)
    auto material_inner_sphere = make_shared<dielectric>(1.0 / 1.5);
    auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    // We create a bunch of spheres
    auto ground_sphere = make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground);
    // Center sphere that is slightly closer to the camera
    auto center_sphere = make_shared<sphere>(point3(0.0, 0.0, -1.2), 0.5, material_center);
    // A sphere we put to the left, which will contain another sphere
    auto outer_left_sphere = make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_outer_sphere);
    // A sphere inside our left sphere
    auto inner_left_sphere = make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.4, material_inner_sphere);
    // A sphere we put to the right
    auto right_sphere = make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right);

    world.add(ground_sphere);
    world.add(center_sphere);
    world.add(inner_left_sphere);
    world.add(outer_left_sphere);
    world.add(right_sphere);
}

void world_with_fov(hittable_list& world) {
    // radius of the spheres
    auto R = cos(pi / 4);

    auto material_left = make_shared<lambertian>(color(0, 0, 1));
    auto material_right = make_shared<lambertian>(color(1, 0, 0));

    world.add(make_shared<sphere>(point3(-R, 0, -1), R, material_left));
    world.add(make_shared<sphere>(point3(R, 0, -1), R, material_right));
}

void twist_my_cam(camera& cam) {
    cam.lookfrom = point3(-2, 2, 1);
    cam.lookat = point3(0, 0, -1);
    cam.vup = vec3(0, 1, 0);
    cam.vfov = 20;
}

int main() {
    // World / Scene configuration
    hittable_list world;

    world_with_spheres(world);

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

    cam.vfov = 90;

    twist_my_cam(cam);

    cam.render(world);
}
