#include <print>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <glm/glm.hpp>

#include "camera.hpp"
#include "scene.hpp"
#include "raytracer.hpp"
#include "rng.hpp"

int main()
{
    Raytracer raytracer(Raytracer::Config{
        .samples = 512,
        .bounces = 64,
        .background = glm::vec3(0.5f, 0.7f,1.0f)
    });

    Camera camera(Camera::Config{
        .width = 1920,
        .height = 1080,
        .v_fov = 20.0,
        .look_from = glm::vec3(13.0f, 2.0f, 3.0f),
        .look_at = glm::vec3(0.0f),
        .v_up = glm::vec3(0.0f, 1.0f, 0.0f),
        .defocus_angle = 0.6f,
        .focus_dist = 10.0f
    });

    Scene scene;

    MaterialID ground = scene.add_lambertian(glm::vec3(0.5f, 0.5f, 0.5f));
    scene.add_sphere(glm::vec3(0.0f, -1000.0f, 0.0f), 1000.0f, ground);

    MaterialID glass = scene.add_dielectric(1.5f);
    scene.add_sphere(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, glass);

    MaterialID lamb = scene.add_lambertian(glm::vec3(0.4f, 0.2f, 0.1f));
    scene.add_sphere(glm::vec3(-4.0f, 1.0f, 0.0), 1.0f, lamb);

    MaterialID metal = scene.add_metal(glm::vec3(0.7f, 0.6f,0.5f), 0.0f);
    scene.add_sphere(glm::vec3(4.0f, 1.0f, 0.0), 1.0f, metal);

    for (i32 a = -11; a < 11; ++a) {
        for (i32 b = -11; b < 11; ++b) {
            f32 choose = Random::float32();
            glm::vec3 center(a + 0.9f * Random::float32(), 0.2f, b + 0.9f * Random::float32());

            if (glm::length(center - glm::vec3(4.0f, 0.2f, 0.0f)) > 0.9f) {
                if (choose < 0.8f) {
                    MaterialID mat = scene.add_lambertian(Random::vec3f() * Random::vec3f());
                    scene.add_sphere(center, 0.2f, mat);
                } else if (choose < 0.95f) {
                    MaterialID mat = scene.add_metal(Random::vec3f(0.5f, 1.0f), Random::float32(0.0f, 0.5f));
                    scene.add_sphere(center, 0.2f, mat);
                } else {
                    MaterialID mat = scene.add_dielectric(1.5f);
                    scene.add_sphere(center, 0.2f, mat);
                }
            }
        }
    }

    std::vector<glm::vec3> buffer = raytracer.render(camera, scene);

    // TODO: better image export (currently 8-bit per channel)
    std::vector<u8> image_buffer;
    image_buffer.reserve(camera.width() * camera.height() * 3);
    for (const auto& pixel : buffer) {
        // tonemapping
        static constexpr auto ACES = [](const glm::vec3& x) -> glm::vec3 {
            static const f32 a = 2.51f;
            static const f32 b = 0.03f;
            static const f32 c = 2.43f;
            static const f32 d = 0.59f;
            static const f32 e = 0.14f;

            return glm::clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0f, 1.0f);
        };
        glm::vec3 colour = ACES(pixel);

        // gamma correction
        static constexpr glm::vec3 gamma(1.0f / 2.2f);
        colour = glm::pow(pixel, gamma);

        image_buffer.emplace_back(static_cast<u8>(colour.r * 255.0f));
        image_buffer.emplace_back(static_cast<u8>(colour.g * 255.0f));
        image_buffer.emplace_back(static_cast<u8>(colour.b * 255.0f));
    }

    if (stbi_write_png("image.png", camera.width(), camera.height(), 3, image_buffer.data(), 3 * camera.width()))
        std::println("render saved to image.png");
}
