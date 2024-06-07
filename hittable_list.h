#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include "traceme.h"

#include <vector>

class hittable_list : public hittable {
    public:
        // We wrap thei every object in the list in a shared pointer, such that we can share
        // ownership of the object. This means that every object is reference counted for every
        // reference other objects have of it. Very similar to Rc in Rust.
        std::vector<shared_ptr<hittable>> objects;

        // Constructors
        hittable_list() {}
        hittable_list(shared_ptr<hittable> object) { add(object); }

        // Adds a new object to the list
        void add(shared_ptr<hittable> object) {
            objects.push_back(object);
        }

        // Clears the list
        void clear() { objects.clear(); }

        // Cast the ray `r` over all the object in the list and return whether or not the ray hit
        // any of the objects in betwee ray_tmin and ray_tmax location ranges.
        // Additionally, it updates the `rec` object with the closest record hit from all the
        // objects in the list.
        bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override {
            // Temporary storeage for the hit record
            hit_record temp_rec;
            // Records whether the ray hit at least 1 object
            bool hit_anything = false;
            // Keep track of the closes object we have hit so far with the ray
            auto closest_so_far = ray_tmax;

            // For each object in the list
            for (const auto& object : objects) {
                // If we hit wit the ray an the object is closer than the previous recorded
                if (object->hit(r, ray_tmin, closest_so_far, temp_rec)) {
                    // We hit something, so we log that
                    hit_anything = true;
                    // Because we hit with the previous interval, this means we now have a closer
                    // object
                    closest_so_far = temp_rec.t;
                    // We update the hit record with the new record
                    rec = temp_rec;
                }
            }
            // Return whether or not we hit something
            return hit_anything;
        }
};

#endif
