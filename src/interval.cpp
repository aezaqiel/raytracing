#include "interval.hpp"

const Interval Interval::Empty = Interval(std::numeric_limits<f32>::infinity(), -std::numeric_limits<f32>::infinity());
const Interval Interval::Universe = Interval(-std::numeric_limits<f32>::infinity(), std::numeric_limits<f32>::infinity());

Interval operator+(const Interval& interval, f32 displacement)
{
    return Interval(interval.min + displacement, interval.max + displacement);
}

Interval operator+(f32 displacement, const Interval& interval)
{
    return Interval(interval.min + displacement, interval.max + displacement);
}
