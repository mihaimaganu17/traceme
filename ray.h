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

        ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction) {}

        // Accessors / Getters
        const point3& origin() const { return orig; }
        const vec3& direction() const { return dir; }

        // Function P(t) described above calculation
        point3 at(double t) const {
            return orig + t * dir;
        }

    private:
        // Origin of the ray
        point3 orig;
        // The 3D direction of the ray
        vec3 dir;
};

#endif
