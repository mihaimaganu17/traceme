#ifndef AABB_H
#define AABB_H

#include "traceme.h"

class aabb {
    public:
        // We will define an interval for all the planes in the 3D world.
        interval x, y, z;

        // The default AABB is empty
        aabb() {}

        // Initializer with user supplied parameters
        aabb(const interval& x, const interval& y, const interval& z): x(x), y(y), z(z) {}

        // Construct a bounding box by treating points `a` and `b` as extremes
        aabb(const point3& a, const point3& b) {
            x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
            y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
            z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
        }

        // Construct a bounding box from 2 other bounding boxes
        aabb(const aabb& box0, const aabb& box1) {
            x = interval(box0.x, box1.x);
            y = interval(box0.y, box1.y);
            z = interval(box0.z, box1.z);
        }

        // Returns the interval over the specified axis `n`
        const interval& axis_interval(int n) const {
            if (n == 1) return y;
            if (n == 2) return z;
            return x;
        }

        // We consided the ray hit the bounding box, if the ranges from all interval that the ray
        // hits do overlap
        bool hit(const ray& r, interval ray_t) const {
            const point3& ray_origin = r.origin();
            const vec3& ray_dir = r.direction();

            // Go through all the 3 dimensions
            for (int axis = 0; axis < 3; axis++) {
                // Get the interval of the current axis
                const interval& ax = axis_interval(axis);
                // Ray is defined by the formula P(t) = Q + t*d.
                // For our interval x (for example), this becomes:
                // x0 = Qx + t0 * d
                // and
                // x1 = Qx + t1 * d
                // Since we have to find the overlap between the intersection of the ray with all
                // of the planes, we have to take out t0 and t1, effectively computing
                // t0 = (x0 - Qx) / d
                // and
                // t1 = (x1 - Qx) / d

                // Get the coordinate corresponding to the current axis from the ray direction and
                // inverse it to compute the 2 `t`s
                const double adinv = 1.0 / ray_dir[axis];

                auto t0 = (ax.min - ray_origin[axis]) * adinv;
                auto t1 = (ax.max - ray_origin[axis]) * adinv;

                // Now we ensure that we make the next comparisons with the overlapping
                // interval between the given `ray_t` and the hit over the current `axis`

                // Test wheather the vector has a positive or a negative direction
                if (t0 < t1) {
                    // If t0 is inside the given interval, we make t0 the new interval minimum
                    if (t0 > ray_t.min) ray_t.min = t0;
                    // If t1 is inside the given interval, we make t1 the new interval maximum
                    if (t1 < ray_t.max) ray_t.max = t1;
                } else {
                    // On this branch, the vector is negative (opposite direction)
                    // If t0 is inside the given interval, we make t0 the new interval maximum
                    if (t0 < ray_t.max) ray_t.max = t0;
                    // If t1 is inside the given interval, we make t1 the new interval minimum
                    if (t1 > ray_t.min) ray_t.min = t1;
                }

                // If the limits overrun one another, there is no overlap, because at least one
                // of them have gotten outside the previous interval
                if (ray_t.max <= ray_t.min)
                    return false;
            }

            // At this point we have overlap and as such, we have a hit
            return true;
        }
};

#endif
