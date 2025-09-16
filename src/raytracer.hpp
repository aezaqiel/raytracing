#pragma once

#include "camera.hpp"
#include "scene.hpp"

class Raytracer
{
public:
    struct Config
    {
        usize samples;
        usize bounces;
        glm::vec3 background; // TODO: hdri skybox
    };

public:
    Raytracer(const Config& config);
    ~Raytracer() = default;

    std::vector<glm::vec3> render(const Camera& cam, const Scene& scene) const;
private:
    usize m_samples { 0 };
    usize m_bounces { 0 };
    glm::vec3 m_background;
};
