#ifndef INTERVAL_H
#define INTERVAL_H

#include "traceme.h"

class interval {
    public:
        // Lower interval bound
        double min;
        // Higher interval bound
        double max;

        // Contructiors
        interval() : min(+infinity), max(-infinity) {}

        interval(double min, double max) : min(min), max(max) {}

        // Return size of the interval
        double size() {
            return max-min;
        }

        // Returns whether of not the given value `x` resides between the internal limits
        bool contains(double x) const {
            return min <= x && x <= max;
        }

        // Returns whether or not the value `x` is inside the open interval
        bool surrounds(double x) const {
            return min < x && x < max;
        }

        // If the value is within `min` and `max`, it is returned. Otherwise, we return the nearest
        // boundary. Basically we return the value that is clamped between the other 2 values from
        // (x, min, max)
        double clamp(double x) const {
            if (x < min) return min;
            if (x > max) return max;
            return x;
        }

        // Predefined intervals
        static const interval empty, universe;
};

// We define the empty interval backwards, such that it is impossible to contains something.
const interval interval::empty(+infinity, -infinity);
// The universe contains everything
const interval interval::universe(-infinity, + infinity);

#endif
