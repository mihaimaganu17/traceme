#ifndef SPHERE_H
#define SPHERE_H

#include "traceme.h"
#include "hittable.h"

class sphere: public hittable {
    public:
        // Constructor for a Stationary Sphere
        sphere(const point3& center, double radius, shared_ptr<material> mat) :
            center1(center), radius(fmax(0, radius)), mat(mat)
        {
            // We define a rectangle as the bounding box (essentially a sphere over the radius)
            auto rvec = vec3(radius, radius, radius);
            // We add a bounding box using that rectangle and the spheres center
            bbox = aabb(center1 - rvec, center1 + rvec);
        }

        aabb bounding_box() const override { return bbox; }

        // Constructor for a Moving Sphere
        sphere(const point3& center1, const point3& center2, double radius,
                shared_ptr<material> mat)
            : center1(center1), radius(fmax(0, radius)), mat(mat), is_moving(true)
        {
            // For a moving sphere, we want the bounds of the entire range of motion trough which
            // the box goes. For this, we want to take the sphere at time t=0 (before movement)
            // and at time t=1 (after movement) and have a rectangle encapsulating both
            auto rvec = vec3(radius, radius, radius);
            aabb bbox1(center1 - rvec, center1 + rvec);
            aabb bbox2(center2 - rvec, center2 + rvec);
            // Make a box of the 2
            bbox = aabb(bbox1, bbox2);
            // Compute the vector along which the sphere moves from center1 to center2
            center_vec = center2 - center1;
        }

        bool hit(const ray& r, const interval& ray_t_interval, hit_record& rec) const override {
            // Updat the center based on the moving ball
            point3 center = is_moving ? sphere_center(r.time()) : center1;
            // We need to solve a*x^2 + b*x + c = 0
            // Vector between the center of the sphere and the origin of the ray cast
            auto ray_to_sphere_center = center - r.origin();
            auto a = dot(r.direction(), r.direction());
            // We actually consider replacing b with (-2h), which gives
            auto h = dot(r.direction(), ray_to_sphere_center);
            auto c = dot(ray_to_sphere_center, ray_to_sphere_center) - radius * radius;

            // We need to compute the term for the square root and check it's sign
            double discriminant = h*h - a * c;

            // If the term is negative, we do not have any real solution so the ray never intersect
            // the sphere
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
            // Compute the texture mapping coordinates u and v
            get_sphere_uv(outward_normal, rec.u, rec.v);
            // Give the hit record information about the material of the surface that was just hit
            rec.mat = mat;

            return true;
        }

    private:
        // In the case of a moving sphere, we want to move it from center1 at time=0 to center2
        // at time = 1. The sphere continues moving indefinitely outside that time interval, so it
        // really can be sampled at any time.
        point3 center1;
        double radius;
        shared_ptr<material> mat;
        // Vector along which the sphere moves from the original point `center1`
        vec3 center_vec;
        // Flag of a moving sphere
        bool is_moving;
        // Bounding box for the sphere
        aabb bbox;

        // Returns the moving sphere's center at the desired `time`
        point3 sphere_center(double time) const {
            // Linearly interpolate from center1 to center2 according to time, where t=0 yields
            // center1, and t=1 yield center2.
            return center1 + time * center_vec;
        }

        // Get the (u,v) coordinates within the 2D texture mapping for a point `p` located on the
        // unit sphere (radius 1), centered at the origin.
        // u: returned value [0, 1] of angle around the Y axis from X = -1;
        // v: returned value [0, 1] of angle from Y = -1 to Y = +1;
        static void get_sphere_uv(const point3& p, double& u, double& v) {
            auto theta = acos(-p.y());
            auto phi = atan2(-p.z(), p.x()) + pi;

            u = phi / (2*pi);
            v = theta / pi;
        }
};
#endif
