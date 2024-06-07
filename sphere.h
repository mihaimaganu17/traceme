#ifndef SPHERE_H
#define SPHERE_H

#include "traceme.h"
#include "hittable.h"

class sphere: public hittable {
    public:
        // Constructor.
        sphere(const point3& center, double radius) : center(center), radius(fmax(0, radius)) {}

        bool hit(const ray& r, const interval& ray_t_interval, hit_record& rec) const override {
            // We need to solve a*x^2 + b*x + c = 0
            // Vector between the center of the sphere and the origin of the ray cast
            auto ray_to_sphere_center = center - r.origin();
            auto a = dot(r.direction(), r.direction());
            // We actually consider replacing b with (-2h), which gives
            auto h = dot(r.direction(), ray_to_sphere_center);
            auto c = dot(ray_to_sphere_center, ray_to_sphere_center) - radius * radius;

            // We need to compute the term for the square root and check it's sign
            double discriminant = h*h - a * c;

            // If the term is negative, we do not have any real solution so the ray never intersect the
            // sphere
            if (discriminant < 0) {
                return false;
            }

            // If the term is zero or positive , there is at least one solution and thus the ray cast
            // intersects the sphere one or 2 points.
            // We know need to find the nearest root that liest between the given t_min and t_max
            // interval
            double sqrt_d = sqrt(discriminant);
            double root = (h - sqrt_d)/ a;

            if (!ray_t_interval.surrounds(root)) {
                // We try with the next root
                root = (h + sqrt_d) / a;
                if (!ray_t_interval.surrounds(root)) {
                    return false;
                }
            }

            // Log the hit record
            rec.p = r.at(root);
            rec.t = root;
            // Compute the normal
            // First we compute the ray vector
            // Then we compute the normal (vec pependicular to the hit point) and normalize it, using
            // the radius of the sphere.
            vec3 outward_normal = (rec.p - center) / radius;
            // Add surface determination for the object
            rec.set_face_normal(r, outward_normal);

            return true;
        }

    private:
        point3 center;
        double radius;
};
#endif
