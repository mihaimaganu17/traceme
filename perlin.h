#ifndef PERLIN_H
#define PERLIN_H

// Header which defines and implements Perlin noise
#include "traceme.h"

class perlin {
    public:
        perlin() {
            // Array to hold the amount of random points we want to generate
            // randfloat = new double[point_count];
            // For each point, we generate a random unit vector.
            randvec = new vec3[point_count];
            // For each point, generate a random double floating point
            for (int i = 0; i < point_count; i++) {
                //randfloat[i] = random_double();
                randvec[i] = unit_vector(vec3::random(-1, 1));
            }

            // For each of the 3D coordinates, generate a random permutation
            perm_x = perlin_generate_perm();
            perm_y = perlin_generate_perm();
            perm_z = perlin_generate_perm();
        }

        // Make sure we deallocate and clean everything in the destructor
        ~perlin() {
            delete[] randfloat;
            delete[] randvec;
            delete[] perm_x;
            delete[] perm_y;
            delete[] perm_z;
        }

        // Create a pseudorandom direction vector from the original point `p`
        double noise(const point3& p) const {
            // In order to make the gradient (in the direction of the vector) smoother, we can
            // interpolate
            // Generate the interpolation weights (could also use higher order polynomial s-curve)
            auto u = p.x() - floor(p.x());
            auto v = p.y() - floor(p.y());
            auto w = p.z() - floor(p.z());

            // Determine the origin coordinates from which we want the vector to start.
            auto i = int(floor(p.x()));
            auto j = int(floor(p.y()));
            auto k = int(floor(p.z()));

            // Structure to hold the values.
            vec3 c[2][2][2];

            // For each of the points in the interpolation, we generate a random float
            for (int di = 0; di < 2; di++)
                for (int dj = 0; dj < 2; dj++)
                    for (int dk = 0; dk < 2; dk++) {
                        // Compute the random gradient for each point
                        c[di][dj][dk] = randvec[
                            perm_x[(i+di) & 255] ^
                            perm_y[(j+dj) & 255] ^
                            perm_z[(k+dk) & 255]
                        ];
                    }
            return perlin_interp(c, u, v, w);
        }

        // Returns a sum of multiple noise frequencies, known as turbulence. Noise is applied
        // `depth` times, each time with a lesser weight than the previous
        double turb(const point3& p, int depth) const {
            auto accum = 0.0;
            auto temp_p = p;
            auto weight = 1.0;

            for (int i = 0; i < depth; i++) {
                accum += weight * noise(temp_p);
                // Bring down the weight
                weight *= 0.5;
                // Go to another point
                temp_p *= 2;
            }

            return fabs(accum);
        }

    private:
        // Perlin noise is repeatable
        static const int point_count = 256;
        vec3* randvec;
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

        static double perlin_interp(vec3 c[2][2][2], double u, double v, double w) {
            auto accum = 0.0;

            // Applying Hermitian smoothing
            auto uu = u*u*(3-2*u);
            auto vv = v*v*(3-2*v);
            auto ww = w*w*(3-2*w);

            for (int i=0; i < 2; i++)
                for (int j=0; j < 2; j++)
                    for (int k=0; k < 2; k++) {
                        // Create the vector of gradient weights
                        vec3 weight_v(u-i, v-j, w-k);
                        accum += (i*uu + (1-i) * (1-uu))
                            * (j*vv + (1-j)*(1-vv))
                            * (k*ww + (1-k)*(1-ww))
                            * dot(c[i][j][k], weight_v);
                    }
            return accum;
        }
};

#endif
