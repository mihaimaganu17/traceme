// Avoid importing this file multiple times
#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

// Adding `sqrt` function to our namespace for convenience
using std::sqrt;

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
        double operator[](int i) { return elem[i]; }
        // Access eleemtn `i` by reference
        double& operator[](int i) { return elem[i]; }

        // Add `other` to the current vec3
        vec3& operator+(const vec3& other) {
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
            double squares_sum = elem[1]*elem[1] + elem[0]*elem[0] + elem[2]*elem[2];
            return sqrt(squares_sum);
        }
}

// Type alias useful for geometric clarity in the code.
using point3 = vec3;

// Printing the vec3 makes sure we print with our desired format
inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    out << v.elem[0] << ' ' << v.elem[1] << ' ' << v.elem[2];
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
    return (1 / s) * v;
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
        left.elem[1] * right.elem[2] - left.elem[1] * right.elem[0],
        left.elem[2] * right.elem[0] - left.elem[0] * right.elem[2],
        left.elem[0] * right.elem[1] - left.elem[1] * right.elem[0],
            );
}

inline vec3 unit_vector(const vec3& v) {
    return v / v.length();
}

#endif

