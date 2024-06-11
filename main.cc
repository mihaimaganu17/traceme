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

void defocus_my_cam(camera& cam) {
    cam.defocus_angle = 10.0;
    cam.focus_dist = 3.4;
}

void random_sphere_cover(hittable_list& world, camera& cam) {
    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto ground_sphere = make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material);

    // Add it to the world
    world.add(ground_sphere);

    // Generate a bunch of random spheres, use `a` as `x` coordinate base and `b` as `z` coordinate
    // base
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            // Stored the likelihood of choosing different materials and and reflection and
            // refraction properties
            auto choose_mat = random_double();

            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            // Only generate the surfaces if we go beyond this random point
            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // Diffuse, most likely to be chosen
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal, less likely
                    auto albedo = color::random(0.5, 1);
                    // How much we reflect
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // Glass, very less likely since is computationally  expensive
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    // Add some big spheres

    // Center sphere
    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    // Left sphere
    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    // Right sphere
    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    // Setup camera
    // Change the aspect ratio to something more popular
    cam.aspect_ratio = 16.0 / 9.0;
    // Change image's width. This will automatically also change the images height as well.
    cam.image_width = 1200;
    // Set the number of ray samples we want to cast for each pixel to do anti-aliasing
    cam.samples_per_pixel = 500;
    // Set the number of times we want the casted rays to reflect on surfaces of the world
    cam.max_depth = 50;

    cam.vfov = 20;

    // Move camera to the right, a bit upwards and a bit backwards
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;
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

    //twist_my_cam(cam);
    //defocus_my_cam(cam);
    random_sphere_cover(world, cam);

    cam.render(world);
}
