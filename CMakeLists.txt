cmake_minimum_required(VERSION 3.30)

project(RT C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(vendor/glm)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/types.hpp
    src/rng.hpp
    src/ray.hpp
    src/interval.hpp
    src/interval.cpp
    src/scene.hpp
    src/scene.cpp
    src/camera.hpp
    src/camera.cpp
    src/raytracer.hpp
    src/raytracer.cpp
)

target_include_directories(${PROJECT_NAME}
PRIVATE
    src
    vendor/stb
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
    glm
)

target_compile_definitions(${PROJECT_NAME}
PRIVATE
    NOMINMAX
    GLM_ENABLE_EXPERIMENTAL
)
