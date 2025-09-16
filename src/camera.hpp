#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

#include "types.hpp"
#include "ray.hpp"

class Camera
{
public:
    struct Config
    {
        usize width;
        usize height;
        f32 v_fov;
        glm::vec3 look_from;
        glm::vec3 look_at;
        glm::vec3 v_up;
        f32 defocus_angle;
        f32 focus_dist;
    };

public:
    Camera(const Config& config);
    ~Camera() = default;

    Ray get_ray(const usize i, const usize j) const;

    inline usize width() const { return m_width; }
    inline usize height() const { return m_height; }

private:
    inline glm::vec3 defocus_sample() const
    {
        glm::vec2 p = glm::diskRand(1.0f);
        return m_position + (p.x * m_defocus_u) + (p.y * m_defocus_v);
    }

private:
    usize m_width { 0 };
    usize m_height { 0 };

    glm::vec3 m_position;

    glm::vec3 m_origin;
    glm::vec3 m_delta_u;
    glm::vec3 m_delta_v;

    f32 m_defocus_angle;
    glm::vec3 m_defocus_u;
    glm::vec3 m_defocus_v;
};
