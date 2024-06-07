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
        // Store whether or not the ray is coming from the inside or outside and whether or not the
        // normal points against the ray or not.
        bool front_face;

        void set_face_normal(const ray& r, const vec3& outward_normal) {
            // Parameter `outward_normal` is assumed to have unit length.
            // If the dot product of the 2 is positive, the ray is inside the sphere
            if (dot(r.direction(), outward_normal) >= 0.0) {
                // We negate the normal such that it does not point against the ray and points
                // outwards
                normal = -outward_normal;
                // We record the fact that the ray is not hitting the front of the surface (but the
                // back rather
                front_face = false;
            } else {
                normal = outward_normal;
                front_face = true;
            }
        }
};

// Defines any surface or volume that can be hit by a ray
class hittable {
    public:
        virtual ~hittable() = default;

        // A hit computed for a casted ray `r`, which only occurs if the result `t` is in between the
        // given interval `tmin` < t < `tmax`
        virtual bool hit(const ray& r, const interval& ray_t_interval, hit_record& rec) const = 0;
};
#endif
