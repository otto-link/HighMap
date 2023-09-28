# HighMap

A C++ library to generate two-dimensional terrain heightmaps for software rendering or video games.

## Warning 

Work in progress, use at your own risk!

![hand_machine](https://github.com/otto-link/HighMap/assets/121820229/6969f3ba-2892-414c-b9d2-6b30847405e3)

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
    - [X] terrain surface from a set of points (thinplate interpolation)
    - [ ] terrain surface from a sketch / splatmap
    - [X] pointwise modifications
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
    - [X] communication network
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
  - [X] distributed / tiled computation
  - [X] multithreading
