#include "scene.hpp"

#include <print>

#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>

#include "rng.hpp"

MaterialID Scene::add_lambertian(const glm::vec3& albedo)
{
    MaterialID id;
    id.type = MaterialType::Lambertian;
    id.index = m_lambertians.count;

    m_lambertians.albedos.push_back(albedo);
    m_lambertians.count++;

    return id;
}

MaterialID Scene::add_metal(const glm::vec3& albedo, f32 fuzz)
{
    MaterialID id;
    id.type = MaterialType::Metal;
    id.index = m_metals.count;

    m_metals.albedos.push_back(albedo);
    m_metals.fuzzes.push_back(fuzz);
    m_metals.count++;

    return id;
}

MaterialID Scene::add_dielectric(f32 refraction_index)
{
    MaterialID id;
    id.type = MaterialType::Dielectric;
    id.index = m_dielectrics.count;

    m_dielectrics.refraction_indices.push_back(refraction_index);
    m_dielectrics.count++;

    return id;
}


void Scene::add_sphere(const glm::vec3& center, f32 radius, const MaterialID& material)
{
    m_spheres.centers.push_back(center),
    m_spheres.radii.push_back(radius);
    m_spheres.materials.push_back(material);
    m_spheres.count++;
}

std::optional<HitRecord> Scene::hit(const Ray& ray, Interval clip) const
{
    std::optional<HitRecord> record;
    HitRecord rec;

    for (usize i = 0; i < m_spheres.count; ++i) {
        const glm::vec3& center = m_spheres.centers[i];
        const f32& radius = m_spheres.radii[i];

        glm::vec3 intersection;
        glm::vec3 normal;
        if (!glm::intersectRaySphere(ray.origin, glm::normalize(ray.direction), center, radius, intersection, normal))
            continue;

        f32 distance = glm::distance(ray.origin, intersection);
        if (!clip.contains(distance))
            continue;

        clip.max = distance;

        rec.position = intersection;
        rec.normal = normal;
        rec.distance = distance;
        rec.material = m_spheres.materials[i];

        record = rec;
    }

    return record;
}

std::optional<ScatterRecord> Scene::scatter(const Ray& ray, const HitRecord& hit) const
{
    std::optional<ScatterRecord> scatter;
    ScatterRecord scat;

    const MaterialID& material = hit.material;

    switch (material.type) {
        case MaterialType::Lambertian: {
            glm::vec3 scatter_direction = hit.normal + Random::unit_vector();

            if (glm::all(glm::lessThan(glm::abs(scatter_direction), glm::vec3(std::numeric_limits<f32>::epsilon()))))
                scatter_direction = hit.normal;

            scat.attenuation = m_lambertians.albedos[material.index];
            scat.scattered = Ray(hit.position, scatter_direction, ray.time);
            scatter = scat;
        } break;
        case MaterialType::Metal: {
            glm::vec3 reflected = glm::reflect(ray.direction, hit.normal);
            reflected = glm::normalize(reflected) + (m_metals.fuzzes[material.index] * Random::unit_vector());

            if (glm::dot(reflected, hit.normal) < 0.0f)
                break;

            scat.attenuation = m_metals.albedos[material.index];
            scat.scattered = Ray(hit.position, reflected, ray.time);
            scatter = scat;
        } break;
        case MaterialType::Dielectric: {
            f32 ri;
            if (glm::dot(ray.direction, hit.normal) > 0.0f)
                ri = m_dielectrics.refraction_indices[material.index];
            else
                ri = 1.0f / m_dielectrics.refraction_indices[material.index];

            glm::vec3 unit_direction = glm::normalize(ray.direction);
            f32 cos_theta = std::fmin(glm::dot(-unit_direction, hit.normal), 1.0f);
            f32 sin_theta = std::sqrt(1.0f - cos_theta * cos_theta);

            static constexpr auto reflectance = [](f32 cosine, f32 ri) {
                f32 r0 = (1.0f - ri) / (1.0f + ri);
                r0 = r0 * r0;
                return r0 + (1.0f - r0) * std::pow((1.0f - cosine), 5.0f);
            };

            glm::vec3 scatter_direction;
            if (ri * sin_theta > 1.0f || reflectance(cos_theta, ri) > Random::float32())
                scatter_direction = glm::reflect(unit_direction, hit.normal);
            else
                scatter_direction = glm::refract(unit_direction, hit.normal, ri);

            scat.attenuation = glm::vec3(1.0f);
            scat.scattered = Ray(hit.position, scatter_direction, ray.time);
            scatter = scat;
        } break;
        default:
            std::println("Undefined material type {}", static_cast<usize>(material.type));
    }

    return scatter;
}
