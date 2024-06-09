// Avoid importing this file multiple times
#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

// Adding `std` library functions to our namespace for convenience
using std::sqrt;
using std::fabs;

// Used to define colors, locations, directions, offsets, whatever.
// Worth trying for homework: Split this into proper classes
class vec3 {
    public:
        // Vector describing 3 elements
        double elem[3];

        // Initializers
        vec3(): elem{0,0,0} {}
        vec3(double e1, double e2, double e3): elem{e1, e2, e3} {}

        // Accessors, which we mark const to describe that `this` cannot be changed by this
        // functions
        double x() const { return elem[0]; }
        double y() const { return elem[1]; }
        double z() const { return elem[2]; }

        // Overloading operators

        // Returns a new vec3 with the values negated
        vec3 operator-() const { return vec3(-elem[0], -elem[1], -elem[2]); }
        // Access element `i`, by copy. hw: add safety checks and unsigned
        double operator[](int i) const { return elem[i]; }
        // Access eleemtn `i` by reference
        double& operator[](int i) { return elem[i]; }

        // Add `other` to the current vec3
        vec3& operator+=(const vec3& other) {
            elem[0] += other.elem[0];
            elem[1] += other.elem[1];
            elem[2] += other.elem[2];
            return *this;
        }

        // Multiply the vector by a scalar `s`
        vec3& operator*=(double s) {
            elem[0] *= s;
            elem[1] *= s;
            elem[2] *= s;
            return *this;
        }

        // Devide by scalar `s`
        vec3& operator/=(double s) {
            // We are just using the multiply by scala overload and multiply by s^(-1)
            return *this *= 1/s;
        }

        // Return the length of the vector
        double length() const {
            return sqrt(length_squared());
        }

        double length_squared() const {
            return elem[0]*elem[0] + elem[1]*elem[1] + elem[2]*elem[2];
        }

        // Generates a vec3 with each element randomized between [0, 1]
        static vec3 random() {
            return vec3(random_double(), random_double(), random_double());
        }

        // Generates a vec3 with each element randomized between [min, max]
        static vec3 random(double min, double max) {
            return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
        }

        // Returns `true` if the vector is very close (in all dimesions) to zero
        bool near_zero() const {
            // A value close to 0
            auto s = 1e-8;
            return (fabs(elem[0]) < s) && (fabs(elem[1]) < s) && (fabs(elem[2]) < s);
        }
};

// Type alias useful for geometric clarity in the code.
using point3 = vec3;

// Printing the vec3 makes sure we print with our desired format
inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.elem[0] << ' ' << v.elem[1] << ' ' << v.elem[2];
}

// Adding 2 vec3 elements and returning a new one
inline vec3 operator+(const vec3& left, const vec3& right) {
    return vec3(
        left.elem[0] + right.elem[0],
        left.elem[1] + right.elem[1],
        left.elem[2] + right.elem[2]);
}

// Substracting 2 vec3 elements and returning a new one
inline vec3 operator-(const vec3& left, const vec3& right) {
    return vec3(
        left.elem[0] - right.elem[0],
        left.elem[1] - right.elem[1],
        left.elem[2] - right.elem[2]);
}

// Multiplying 2 vec3 elements and returning a new one
inline vec3 operator*(const vec3& left, const vec3& right) {
    return vec3(
        left.elem[0] * right.elem[0],
        left.elem[1] * right.elem[1],
        left.elem[2] * right.elem[2]);
}

// Right multiply vec3 by scalar returning a new vec3
inline vec3 operator*(const vec3& v, double s) {
    return vec3(v.elem[0] * s, v.elem[1] * s, v.elem[2] * s);
}

// Left multiply vec3 by scalar returning a new vec3
inline vec3 operator*(double s, const vec3& v) {
    return v * s;
}

// Division by scalar value
inline vec3 operator/(const vec3& v, double s) {
    return v * (1.0 / s);
}

// Dot product of 2 vec3's
inline double dot(const vec3& left, const vec3& right) {
    return left.elem[0] * right.elem[0]
        + left.elem[1] * right.elem[1]
        + left.elem[2] * right.elem[2];
}

// Cross product of 2 vec3's
// Cross-products have real-life applications in Physics. These are:
// (1) torque on a wrench,
// (2) magnetic force on a moving electric charge
// (3) angular momentum of a rotating object.
inline vec3 cross(const vec3& left, const vec3& right) {
    return vec3(
        left.elem[1] * right.elem[2] - left.elem[2] * right.elem[1],
        left.elem[2] * right.elem[0] - left.elem[0] * right.elem[2],
        left.elem[0] * right.elem[1] - left.elem[1] * right.elem[0]
            );
}

inline vec3 unit_vector(const vec3& v) {
    return v / v.length();
}

// We use the rejection method to generate a random vector inside of the unit sphere.
// We pick a random point in the unit cube and reject it until is also in the unit sphere.
inline vec3 random_in_unit_sphere() {
    // Keep going
    while (true) {
        // Pick a random point in the unit cube
        auto p = vec3::random(-1, 1);
        // The unit sphere has a radiu of 1
        auto unit_sphere_radius = 1.0;
        // We consider the center of the sphere (sx, sy, sz)
        // A point (x, y, z) is in the unit sphere if it satisfies:
        // (x - sx)^2 + (y - sy)^2 + (z - sz)^2 < r^2
        // In our case, the unit sphere center point is (0, 0, 0), so it becomes
        // x^2 + y^2 + z^2 < r^2. Which is essentially the dot product of p with p < 1
        if (p.length_squared() < 1.0) {
            return p;
        }
    }
}

// We normalize the random vector inside a sphere to obtain the point ON the sphere. Which
// essentially is solving `r` for r = sqrt(dot(p, p))
inline vec3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}

// Determine if the vector is on the right hemisphere. We can do that by comparing it with the
// vector normal. If their dot product is positive, they are on the same side of the hemisphere.
// If it is negative, then they are on diferent sides and we need to invert the vector.
inline vec3 random_on_hemisphere(const vec3& normal) {
    auto random_reflect_ray = random_unit_vector();

    if (dot(random_reflect_ray, normal) > 0.0) {
        return random_reflect_ray;
    } else {
        // invert the vector so we are on the same side as the normal
        return -random_reflect_ray;
    }
}

/*
// Computes the reflection vector for a metal surface.
// TODO: The parameters of this function can easily be interchanged, which will cause major havoc
// in the ray business.
inline vec3 reflect(const vec3& v, const vec3& normal) {
    // First we find the length of the vector, using the dot product
    double refl_length = dot(v, normal);
    // Then, we need tranform this lenght in a vector
    vec3 b_vec_towards_ray = refl_length * normal;
    // Now, we want to go out in the opposite direction, so we negate the vector to inverse it. We
    // also need to double the vector to compute its extension out of the surface and not just from
    // inside until it hits the surface.
    auto b_vec_reflect = v -2 * b_vec_towards_ray;
    // Vec addition rule
    return b_vec_reflect;
}
*/

inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2*dot(v,n)*n;
}

#endif
