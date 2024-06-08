#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

// Tell the compiler this is a class that will be defined later (in material.h). This solves a
// circular reference issue, where hit_record and material need to keep a reference of each other.
class material;

// Logs the occurence of a single hit from the intersection of a ray cast and a surface or volume
// (an object in the scene)
class hit_record {
    public:
        // Point where the ray hit the object
        point3 p;
        // Normal at the intersection point
        vec3 normal;
        // The material of the surface that we hit.
        shared_ptr<material> mat;
        // Gives the length of the ray that hit
        double t;
        // Records whether or not the ray hit the surface from the outside (true) or the inside
        // (false)
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
