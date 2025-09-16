#include "raytracer.hpp"

#include <print>
#include <atomic>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <execution>

Raytracer::Raytracer(const Config& config)
{
    m_samples = config.samples;
    m_bounces = config.bounces;
    m_background = config.background;
}

std::vector<glm::vec3> Raytracer::render(const Camera& cam, const Scene& scene) const
{
    auto start = std::chrono::high_resolution_clock::now();

    static std::atomic<usize> lines_done = 0;
    static std::atomic<i32> last_percent = -1;
    static std::mutex progress_mutex;

    static auto print_progress = [&](i32 percent, usize done, usize total) {
        std::lock_guard<std::mutex> lock(progress_mutex);
        static const i32 bar_width = 50;
        i32 pos = (percent * bar_width) / 100;
        std::cout << '\r' << '[';
        for (i32 i = 0; i < bar_width; ++i)
            std::cout << (i < pos ? '#' : '-');
        std::cout << "] " << std::setw(3) << percent << "% (" << done << "/" << total << ")";
        std::flush(std::cout);
        if (percent >= 100) std::cout << std::endl;
    };

    const usize width = cam.width();
    const usize height = cam.height();
    std::vector<glm::vec3> buffer(width * height, glm::vec3(0.0f));

    std::vector<usize> scanlines(height);
    for (usize i = 0; i < height; ++i)
        scanlines[i] = i;

    std::for_each(std::execution::par, scanlines.begin(), scanlines.end(), [&](usize j) {
        for (usize i = 0; i < width; ++i) {
            glm::vec3 colour(0.0f);

            for (usize s = 0; s < m_samples; ++s) {
                Ray ray = cam.get_ray(i, j);

                glm::vec3 light(1.0f);

                for (usize b = 0; b < m_bounces; ++b) {
                    if (auto hit = scene.hit(ray, Interval(0.001f, std::numeric_limits<f32>::infinity()))) {
                        if (auto scatter = scene.scatter(ray, hit.value())) {
                            light *= scatter->attenuation;
                            ray = scatter->scattered;
                        } else {
                            light = glm::vec3(0.0f);
                            break;
                        }
                    } else {
                        light *= m_background;
                        break;
                    }
                }

                colour += light;
            }

            buffer[i + j * width] = colour / static_cast<f32>(m_samples);
        }

        const usize done = lines_done.fetch_add(1, std::memory_order_relaxed) + 1;
        const i32 percent = static_cast<i32>((done * 100) / height);
        i32 prev = last_percent.load(std::memory_order_relaxed);
        if (percent > prev) {
            if (last_percent.compare_exchange_strong(prev, percent, std::memory_order_relaxed))
                print_progress(percent, done, height);
        }
    });

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<f64, std::milli> duration = end - start;
    std::println("render time: {:.2f} s", duration.count() / 1000.0);

    return buffer;
}
