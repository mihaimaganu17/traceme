#ifndef BVH_H
#define BVH_H

#include "traceme.h"

#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"

#include <algorithm>

// Node representing a tree of bouning volume hierarchies
class bvh_node: public hittable {
    public:
        // Create a new node with a hittable list
        bvh_node(hittable_list list): bvh_node(list.objects, 0, list.objects.size()) {
            // This constructor creates a copy of the hittable list (not ideal), which we will
            // modify. The lifetime of the copied list only extends until this constructor exits.
            // That is OK, because we only need to persist the resulting bounding bolume hierarchy
        }

        // The most complicated part of any efficiency structure is building it. We construct a
        // new `bvh_node` from a given list of object within the span given by `start` and `end`
        // offsets
        bvh_node(std::vector<shared_ptr<hittable>>& objects, size_t start, size_t end) {
            // Build the bounding box from the span of the source objects
            bbox = aabb::empty;

            for (size_t object_index=start; object_index < end; object_index++) {
                bbox = aabb(bbox, objects[object_index]->bounding_box());
            }

            // And we split based on the longest axis, to give us a better split
            unsigned int axis = bbox.longest_axis();
            // Comparator for the sorting function. Based on what axis we choose, we compare that
            // axis
            auto comparator  = (axis == 0) ? box_x_compare
                            : (axis == 1) ? box_y_compare
                            : box_z_compare;

            auto object_span = end - start;

            // If we have a single object, we put it in both trees to avoid treating null pointer
            // conditions
            if (object_span == 1) {
                left = right = objects[start];
            } else if (object_span == 2) {
                // If we have 2 objects, we put one in the left tree and one in the right tree
                left = objects[start];
                right = objects[start+1];
            } else {
                // We sort the objects with the chosen comparison function
                std::sort(objects.begin() + start, objects.begin() + end, comparator);

                // Chose  mid point
                auto mid = start + object_span / 2;

                left = make_shared<bvh_node>(objects, start, mid);
                right = make_shared<bvh_node>(objects, mid + 1, end);
            };
        }

        // Computes whether the ray hits this node, by recursively checking its left and right
        // children
        bool hit(const ray& r, const interval& ray_t, hit_record& rec) const override {
            // If the box that represents this object is not hit, there is no need to check the
            // children
            if (!bbox.hit(r, ray_t))
                return false;

            // Check if we hit the left tree
            bool hit_left = left->hit(r, ray_t, rec);
            // If we already hit the left tree, we have to update the interval we check for
            bool hit_right = right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

            return hit_left || hit_right;
        }

        aabb bounding_box() const override { return bbox; }

    private:
        // Left tree of this node
        shared_ptr<hittable> left;
        // Right tree of this node
        shared_ptr<hittable> right;
        // Bounding box for this node
        aabb bbox;

        // Compares the 2 objects `a` and `b` based on the minimum value over the `axis_index`
        static bool box_compare(
            const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis_index
        ) {
            auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
            auto b_axis_interval = b->bounding_box().axis_interval(axis_index);
            return a_axis_interval.min < b_axis_interval.min;
        }

        static bool box_x_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
            return box_compare(a, b, 0);
        }

        static bool box_y_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
            return box_compare(a, b, 1);
        }

        static bool box_z_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
            return box_compare(a, b, 2);
        }
};

#endif
