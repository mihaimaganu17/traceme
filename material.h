#ifndef MATERIAL_H
#define MATERIAL_H

#include "traceme.h"

class hit_record;

// Abstract class that encapsulates what behaviour a material is expected to have. A material needs
// to be able to do the following things:
// 1. Produce a scattered ray (or say it absorved the incident ray).
// 2. If scattered, say how much the ray should be attenuated. (how much it should reflect)
class material {
    public:
        // Destructor
        virtual ~material() = default;

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const {
            return false;
        }
}

// Modeling light Scatter and Reflectance

// Lambertian (diffuse) reflectance can either always scatter and attenuate light according to its
// reflectance 𝑅, or it can sometimes scatter (with probabilty 1−𝑅) with no attenuation (where a
// ray that isn't scattered is just absorbed into the material). It could also be a mixture of both
// those strategies. We will choose to always scatter, so implementing Lambertian materials becomes
// a simple task

class lambertian: public material {
    public:
        lambertian(const color& albedo): albedo(albedo) {}

        bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override {
            // Direction of the scattered / reflected ray
            auto scatter_direction = rec.normal + random_unit_vector();

            // There is the very possible case that the random unit vector we generate above is
            // totally opposite of the normal vector. As such, they will sum to zero, which will
            // result in zero scatter direction vector. We want to catch that and just replace it
            // with the normal, if it happens
            if (scatter_direction.near_zero()) {
                scatter_direction = rec.normal;
            }
            // Construct the ray that gets scattered (reflected)
            scattered = ray(rec.p, scatter_direction);
            // Communicate our attenuation (albedo) or the fractional reflectance
            // Another option would be to scatter with a certain probability `p` and then we would
            // have the `attenuation = albedo / p`
            attenuation = albedo;
            // Communicated that we did scatter / reflected the ray
            return true;
        }

    private:
        // Albedo (Latin for whiteness) is used here to be the `fractional reflectance`, which
        // in essence is the ray that is reflected off of the surface, given the ray incidence, the
        // color and the material of the surface.
        color albedo;
}

#endif
