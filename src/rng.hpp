#pragma once

#include <random>
#include <limits>

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "types.hpp"

class Random
{
public:
    inline static u32 uint32()
    {
        return s_distribution(s_engine);
    }

    inline static u32 uint32(u32 min, u32 max)
    {
        return min + (max - min + 1) * uint32();
    }

    inline static f32 float32()
    {
        return static_cast<f32>(s_distribution(s_engine)) / static_cast<f32>(std::numeric_limits<u32>::max());
    }

    inline static f32 float32(f32 min, f32 max)
    {
        return min + (max - min) * float32();
    }

    inline static glm::vec3 vec3f()
    {
        return glm::vec3(float32(), float32(), float32());
    }

    inline static glm::vec3 vec3f(f32 min, f32 max)
    {
        return glm::vec3(float32(min, max), float32(min, max), float32(min, max));
    }

    inline static glm::vec3 unit_vector()
    {
        while (true) {
            glm::vec3 p = vec3f(-1.0f, 1.0f);
            if (glm::length2(p) < 1.0f)
                return glm::normalize(p);
        }
    }

private:
    inline static thread_local std::mt19937 s_engine;
    inline static thread_local std::uniform_int_distribution<std::mt19937::result_type> s_distribution;
};
