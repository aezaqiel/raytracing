#include "camera.hpp"

#include "rng.hpp"

Camera::Camera(const Config& config)
{
    m_width = config.width;
    m_height = config.height;
    m_position = config.look_from;
    m_defocus_angle = config.defocus_angle;

    f32 theta = glm::radians(config.v_fov);
    f32 h = std::tan(theta / 2.0f);

    f32 aspect_ratio = static_cast<f32>(m_width) / static_cast<f32>(m_height);

    f32 viewport_height = 2.0f * h * config.focus_dist;
    f32 viewport_width = viewport_height * aspect_ratio;

    glm::vec3 w(glm::normalize(config.look_from - config.look_at));
    glm::vec3 u(glm::normalize(glm::cross(config.v_up, w)));
    glm::vec3 v(glm::cross(w, u));

    glm::vec3 viewport_u = viewport_width * u;
    glm::vec3 viewport_v = viewport_height * -v;

    m_delta_u = viewport_u / static_cast<f32>(m_width);
    m_delta_v = viewport_v / static_cast<f32>(m_height);

    glm::vec3 origin = m_position - (config.focus_dist * w) - viewport_u / 2.0f - viewport_v / 2.0f;
    m_origin = origin + 0.5f * (m_delta_u + m_delta_v);

    f32 defocus_radius = config.focus_dist * std::tan(glm::radians(config.defocus_angle / 2.0f));
    m_defocus_u = u * defocus_radius;
    m_defocus_v = v * defocus_radius;
}

Ray Camera::get_ray(const usize i, const usize j) const
{
    glm::vec2 offset(Random::float32(-0.5f, 0.5f), Random::float32(-0.5f, 0.5f));

    glm::vec3 pixel_sample = m_origin + (static_cast<f32>(i) + offset.x) * m_delta_u + (static_cast<f32>(j) + offset.y) * m_delta_v;

    glm::vec3 ray_origin = (m_defocus_angle <= 0.0f) ? m_position : defocus_sample();
    glm::vec3 ray_direction = pixel_sample - ray_origin;

    return Ray(ray_origin, ray_direction, Random::float32());
}
