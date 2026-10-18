# Quickstart

This guide shows how to generate and export a heightmap using HighMap.

## 1. Example

The following program generates a 2D elevation grid using Perlin fractal Brownian motion (fBm), remaps its values to the interval $[0, 1]$, and writes the result to a 16-bit PNG file.

```cpp
#include <iostream>
#include "highmap.hpp"

int main()
{
  glm::ivec2 shape = {512, 512};
  glm::vec2 res = {4.0f, 4.0f};
  int seed = 42;

  // Generate elevation data
  hmap::Array z = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, res, seed);

  std::cout << "min: " << z.min() << "\n";
  std::cout << "max: " << z.max() << "\n";

  // Remap elevation values to [0, 1]
  z.remap(0.0f, 1.0f);

  // Export to image
  hmap::ImageWriterConfig config;
  config.data_type = hmap::ImageDataType::UINT16;
  hmap::export_image(z, "terrain.png", config);

  return 0;
}
```

## 2. CMake Integration

Link against HighMap using CMake:

```cmake
cmake_minimum_required(VERSION 3.20)
project(my_project CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(HighMap REQUIRED)

add_executable(my_project main.cpp)
target_link_libraries(my_project PRIVATE HighMap::HighMap)
```

## 3. Basic Operations

Common operations on an `Array`:

- **Smoothing**:
  ```cpp
  z = hmap::smooth(z, 3);
  ```

- **Clamping**:
  ```cpp
  z.clamp(0.0f, 1.0f);
  ```

- **Blending**:
  ```cpp
  hmap::Array overlay = hmap::noise_fbm(hmap::NoiseType::PERLIN, shape, {8.f, 8.f}, 101);
  z = hmap::blend_max(z, overlay * 0.5f);
  ```

## 4. Next Steps

- [Core Concepts](core_concepts.md)
