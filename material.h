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
};

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
};

// Implements `material` class for a metal material
class metal: public material {
    public:
        metal(const color& albedo, double fuzz): albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

        // r_in -> ray we casted
        // hit -> point where the ray hit the surface (in our case this material)
        // attenuation -> factor returning how much we should reflect
        // scattered -> the reflected, scattered ray from the hit point
        bool scatter(const ray& r_in, const hit_record& hit, color& attenuation, ray& scattered)
        const override {
            // Compute the reflected vector
            vec3 reflected = reflect(r_in.direction(), hit.normal);
            // Normalize the reflected ray
            vec3 reflected_unit = unit_vector(reflected);
            // Create a new fuzz vector for the unit sphere
            vec3 fuzz_vec = fuzz * random_unit_vector();
            // Assign a new vector (vector addition), where we should fuzz the ray
            reflected = reflected_unit + fuzz_vec;
            // Construct a ray using it and the hit point of the previous ray
            scattered = ray(hit.p, reflected);
            // Assign our desired attenuation
            attenuation = albedo;

            // If the length of the vector is negative, we are scaterring below the surface, and 
            // we just absord that
            return (dot(scattered.direction(), hit.normal) > 0);
        }
    private:
        // See description in `material.h:lambertian` class under the same name
        color albedo;
        // The fuzz parameters is the radius of the fuzz sphere that we use to fuzz reflections.
        // When a ray is reflected, we consider the end point of that vector, the center of the
        // fuzz sphere. The `fuzz` parameter below is the radius of that sphere. Using these 2
        // informations, we chose another point on the sphere to the vector to end, such that
        // the reflection is now fuzzed.
        double fuzz;
};

class dielectric : public material {
    public:
        dielectric(double refraction_index) : refraction_index(refraction_index) {}

        bool scatter(const ray& r_in, const hit_record& hit, color& attenuation, ray& scattered)
        const override {
            // We make refraction, refract the exact same color of light
            attenuation = color(1.0, 1.0, 1.0);
            // If we hit the front of the surface, we raise to -1
            double ri = hit.front_face ? (1.0/refraction_index) : refraction_index;

            // Compute the unit vector of the incomming ray
            vec3 unit_direction = unit_vector(r_in.direction());
            // Compute the refraction
            vec3 refracted = refract(unit_direction, hit.normal, ri);

            // Scatter the refracted ray
            scattered = ray(hit.p, refracted);

            return true;
        }

    private:
        double refraction_index;
};

#endif
