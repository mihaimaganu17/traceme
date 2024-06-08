// Include our own defined headers
#include "traceme.h"
#include "camera.h"
#include "sphere.h"
#include "hittable.h"
#include "hittable_list.h"


int main() {
    // World / Scene configuration
    hittable_list world;

    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    // Set up the camera through which we view the world
    camera cam;

    // Change the aspect ratio to something more popular
    cam.aspect_ratio = 16.0 / 9.0;
    // Change image's width. This will automatically also change the images height as well.
    cam.image_width = 400;
    // Set the number of ray samples we want to cast for each pixel to do anti-aliasing
    cam.samples_per_pixel = 100;

    cam.render(world);
}
