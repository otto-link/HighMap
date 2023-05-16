# HighMap

A C++ library to generate two-dimensional terrain heightmaps for software rendering or video games.

[toc]

## License

This project is licensed under the GNU General Public License v3.0.

## Getting started

### Building

Build by making a build directory (i.e. `build/`), run `cmake` in that dir, and then use `make` to build the desired target.

Example:
``` bash
mkdir build && cd build
cmake ..
make
```

Simple examples are provided for most of the functions, for instance:
```
bin/./ex_fbm_perlin
eog ex_fbm_perlin.png
```

### Usage examples

### Documentation

## Development roadmap

- Heightmap generation and alteration
  - [X] procedural noise (Perlin, fractal...)
  - controlled generation
    - [ ] terrain surface from a set of points (thinplate interpolation)
    - [ ] terrain surface from a sketch / splatmap
    - [ ] shape-conservative pointwise modifications (i.e. enforce elevation at some locations while keeping the overall shape of the heightmap)
- Physics-based mechanisms
  - erosion / deposition
    - [X] thermal
    - [X] hydraulic
    - [X] sediment deposition
    - [ ] wind
  - hydrology
    - [ ] surface water system (stream and pool maps)
    - [ ] downcutting (vertical erosion of stream's bed or valley's floor)
    - [ ] heightmap fixing to ensure unbroken flow streams
    - [ ] snow deposition
    - [ ] ice floe
    - [ ] lava/glacier viscous flow
- Biomes
  - micro-climate
    - [ ] dominant wind
    - [ ] precipitation map
    - [ ] snowfall map
    - [ ] sun exposure
  - [ ] soil type
  - [ ] vegetation (flora)
  - [ ] animals (fauna)
- Object automatic placement
  - [ ] rocks, boulders, cliffs...
  - [ ] trees
  - [ ] cities
- Anthropic features
  - roads / paths
    - [ ] communication network
    - [ ] path/road digging and leveling
  - cities
    - [ ] ground leveling
    - [ ] inner structure (streets)
  - [ ] agriculture
- UI
  - [ ] splatmap generation
  - [ ] multiscale terrain class
  - [ ] gateways to Unity, Unreal Engine...
- Performances
  - [ ] distributed / tiled computation
  - [ ] multithreading
