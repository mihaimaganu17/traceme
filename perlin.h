#ifndef PERLIN_H
#define PERLIN_H

// Header which defines and implements Perlin noise
#include "traceme.h"

class perlin {
    public:
        perlin() {
            // Array to hold the amount of random points we want to generate
            randfloat = new double[point_count];
            // For each point, generate a random double floating point
            for (int i = 0; i < point_count; i++) {
                randfloat[i] = random_double();
            }

            // For each of the 3D coordinates, generate a random permutation
            perm_x = perlin_generate_perm();
            perm_y = perlin_generate_perm();
            perm_z = perlin_generate_perm();
        }

        // Make sure we deallocate and clean everything in the destructor
        ~perlin() {
            delete[] randfloat;
            delete[] perm_x;
            delete[] perm_y;
            delete[] perm_z;
        }

        // Create a pseudorandom direction vector from the original point `p`
        double noise(const point3& p) const {
            // Generate a random offset for each coordinate of the 3D space
            auto i = int(4*p.x()) & 255;
            auto j = int(4*p.y()) & 255;
            auto k = int(4*p.z()) & 255;

            return randfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
        }

    private:
        // Perlin noise is repeatable
        static const int point_count = 256;
        double* randfloat;
        int* perm_x;
        int* perm_y;
        int* perm_z;

        // Generate a random permutation of values from 0 to point_count
        static int* perlin_generate_perm() {
            auto p = new int[point_count];
            for (int i = 0; i < point_count; i++) {
                p[i] = i;
            }

            permute(p, point_count);
            return p;
        }

        // Permutates of elements from position 0 to `n` in the p array
        static void permute(int* p, int n) {
            for (int i = n-1; i > 0; i--) {
                // Choose a random position in the array
                int target = random_int(0, i);
                // Swap positions of the element at the random position with the element at the
                // current position
                int tmp = p[i];
                p[i] = p[target];
                p[target] = tmp;
            }
        }
};

#endif
