// All ray tracers have a ray class and a computation of what color is seen along a ray.
//
// P(t) = A + t*b
// P - 3D position along a line in 3D.
// A - the ray origin
// b - the direction of the ray (similar to the slope of derivatives)
#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
    public:
        ray() {}

        ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction), tm(0) {}

        ray(const point3& origin, const vec3& direction, double tm)
            : orig(origin), dir(direction), tm(tm) {}

        // Accessors / Getters
        const point3& origin() const { return orig; }
        const vec3& direction() const { return dir; }

        double time() const { return tm; }

        // Function P(t) described above calculation
        point3 at(double t) const {
            return orig + t * dir;
        }

    private:
        // Origin of the ray
        point3 orig;
        // The 3D direction of the ray
        vec3 dir;
        // In a real camera, the shutter remains open for a short time interval, during which the
        // camera and objects in the world may move. To accurately reproduce such a camera shot, we
        // seek an average of what the camera senses while its shutter is open to the world.
        //
        // We can get a random estimate of a single (simplified) photon by sending a single ray at
        // some random instant in time while the shutter is open.
        // If we can determine where the objects are supposed to be at that instant, we can get an
        // accurate measure of the light for that ray at that same instant.
        // `tm` stores the exact time (instante) for each ray.
        double tm;
};

#endif
