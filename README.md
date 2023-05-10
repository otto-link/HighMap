# HighMap

A C++ library to generate two-dimensional terrain heightmaps for software rendering or video games.

[toc]

## License

This project is licensed under the GNU General Public License v3.0.

## Getting started

### Installation

### Usage examples

### Documentation

# Development roadmap

- Heightmap generation and alteration
  - [ ] procedural noise (Perlin, fractal...)
  - controlled generation
    - [ ] terrain surface from a set of points (thinplate interpolation)
    - [ ] terrain surface from a sketch / splatmap
    - [ ] shape-conservative pointwise modifications (i.e. enforce elevation at some locations while keeping the overall shape of the heightmap)
- Physics-based mechanisms
  - erosion / deposition
    - [ ] thermal
    - [ ] hydraulic
    - [ ] sediment deposition
    - [ ] wind / in progress
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
