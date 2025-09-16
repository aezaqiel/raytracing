#pragma once

#include <vector>
#include <optional>

#include <glm/glm.hpp>

#include "types.hpp"
#include "ray.hpp"
#include "interval.hpp"

enum class MaterialType
{
    Lambertian,
    Metal,
    Dielectric
};

struct MaterialID
{
    MaterialType type;
    usize index;
};

struct HitRecord
{
    glm::vec3 position;
    glm::vec3 normal;
    f32 distance;
    MaterialID material;
};

struct ScatterRecord
{
    glm::vec3 attenuation;
    Ray scattered;
};

class Scene
{
public:
    Scene() = default;
    ~Scene() = default;

    MaterialID add_lambertian(const glm::vec3& albedo);
    MaterialID add_metal(const glm::vec3& albedo, f32 fuzz);
    MaterialID add_dielectric(f32 refraction_index);

    void add_sphere(const glm::vec3& center, f32 radius, const MaterialID& material);

    std::optional<HitRecord> hit(const Ray& ray, Interval clip) const;
    std::optional<ScatterRecord> scatter(const Ray& ray, const HitRecord& hit) const;

private:
    struct LambertianCollections
    {
        usize count { 0 };
        std::vector<glm::vec3> albedos;
    };

    struct MetalCollections
    {
        usize count { 0 };
        std::vector<glm::vec3> albedos;
        std::vector<f32> fuzzes;
    };

    struct DielectricCollections
    {
        usize count { 0 };
        std::vector<f32> refraction_indices;
    };

    struct SphereCollections
    {
        usize count { 0 };
        std::vector<glm::vec3> centers;
        std::vector<f32> radii;
        std::vector<MaterialID> materials;
    };

private:
    LambertianCollections m_lambertians;
    MetalCollections m_metals;
    DielectricCollections m_dielectrics;

    SphereCollections m_spheres;
};
