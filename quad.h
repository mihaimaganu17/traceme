#ifndef QUAD_H

// Defines a quadrilateral

#define QUAD_H

#include "traceme.h"
#include "hittable.h"

// Class defining a quadrilateral
class quad: public hittable {
    public:
        quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat)
            : Q(Q), u(u), v(v), mat(mat)
        {
            // Compute the normal, as the perpendicular vector of the plane made by `u` and `v`
            auto n = cross(u, v);
            // Transform it into a unit vector
            normal = unit_vector(n);
            // Compute D
            D = dot(normal, Q);
            w = n / dot(n, n);
            set_bounding_box();
        }

        virtual void set_bounding_box() {
            // Compute the bounding box of all four vertices;
            auto bbox_diagonal1 = aabb(Q, Q + u + v);
            auto bbox_diagonal2 = aabb(Q + u, Q + v);

            bbox = aabb(bbox_diagonal1, bbox_diagonal2);
        }

        aabb bounding_box() const override { return bbox; }

        bool hit(const ray& r, const interval& ray_t_interval, hit_record& rec) const override {
            // Compute denominator
            auto denom = dot(normal, r.direction());

            // No hit is the ray is parallel to the plane
            if (fabs(denom) < 1e-8) {
                return false;
            }

            // Return false if the hit point parameter t is outside the ray interval.
            auto t = (D - dot(normal, r.origin())) / denom;
            if (!ray_t_interval.contains(t)) return false;

            // Determine if the hit point lies withing the planar shape using its plane coordinates.
            auto intersection = r.at(t);

            vec3 planar_hitpt_vector = intersection - Q;
            auto alpha = dot(w, cross(planar_hitpt_vector, v));
            auto beta = dot(w, cross(u, planar_hitpt_vector));

            if (!is_interior(alpha, beta, rec))
                return false;

            // Ray hits the 2D shape; set the rest of the hit record and return true.
            rec.t = t;
            rec.p = intersection;
            rec.mat = mat;
            rec.set_face_normal(r, normal);

            return true;
        }

        // Computes whether or not the point defined by `a` and `b` on the plane is contained
        // inside the unit interval
        virtual bool is_interior(double a, double b, hit_record& rec) const {
            interval unit_interval = interval(0, 1);

            if (!unit_interval.contains(a) || !unit_interval.contains(b))
                return false;

            rec.u = a;
            rec.v = b;

            return true;
        }

    private:
        // Original edge / corner
        point3 Q;
        // Vectors for the 2 sides starting from the origin
        vec3 u, v;
        // Material used by this shape
        shared_ptr<material> mat;
        // Bounding box for this current shape
        aabb bbox;
        // This is just a constant used to compute if a point is contained within the plane
        double D;
        // Normal on the surface
        vec3 normal;
        vec3 w;
};

#endif
