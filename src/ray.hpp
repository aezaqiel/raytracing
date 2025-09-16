#pragma once

#include <glm/glm.hpp>

#include "types.hpp"

struct Ray
{
    glm::vec3 origin;
    glm::vec3 direction;
    f32 time;

    Ray() = default;
    Ray(const glm::vec3& o, const glm::vec3& d, f32 t = 0.0f)
        : origin(o), direction(d), time(t) {}

    inline glm::vec3 at(f32 t) const
    {
        return origin + direction * t;
    }
};
