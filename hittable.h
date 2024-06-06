#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

// Logs the occurence of a single hit from the intersection of a ray cast and a surface or volume
// (an object in the scene)
class hit_record {
    public:
        point3 p;
        vec3 normal;
        double t;
};

// Defines any surface or volume that can be hit by a ray
class hittable {
    public:
        virtual ~hittable() = default;

        // A hit computed for a casted ray `r`, which only occurs if the result `t` is in between the
        // given interval `tmin` < t < `tmax`
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};
#endif
