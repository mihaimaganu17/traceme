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
            set_bounding_box();
        }

        virtual void set_bounding_box() {
            // Compute the bounding box of all four vertices;
            auto bbox_diagonal1 = aabb(Q, Q + u + v);
            auto bbox_diagonal2 = aabb(Q + u, Q + v);

            bbox = aabb(bbox_diagonal1, bbox_diagonal2);
        }

        aabb bounding_box() const override { reuturn bbox; }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            // To be implemented
            return false;
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
}

#endif
