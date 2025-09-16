#pragma once

#include <limits>
#include <algorithm>

#include "types.hpp"

struct Interval
{
    f32 min;
    f32 max;

    Interval()
        : min(std::numeric_limits<f32>::infinity()), max(-std::numeric_limits<f32>::infinity()) {}
    
    Interval(f32 min, f32 max)
        : min(min), max(max) {}
    
    Interval(const Interval& a, const Interval& b)
    {
        min = std::min(a.min, b.min);
        max = std::max(a.max, b.max);
    }

    inline f32 size() const { return max - min; }
    inline bool contains(f32 x) const { return min <= x && x <= max; }
    inline bool surrounds(f32 x) const { return min < x && x < max; }
    inline f32 clamp(f32 x) const { return std::clamp(x, min, max); }
    
    Interval expand(f32 delta) const
    {
        f32 padding = delta / 2.0f;
        return Interval(min - padding, max + padding);
    }

    static const Interval Empty;
    static const Interval Universe;
};

static Interval operator+(const Interval& interval, f32 displacement);
static Interval operator+(f32 displacement, const Interval& interval);
