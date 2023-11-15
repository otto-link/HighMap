# HighMap

A C++ library to generate two-dimensional terrain heightmaps for software rendering or video games. HighMap is the core solver of the [Hesiod](https://github.com/otto-link/Hesiod) GUI.

![ex_hydraulic_vpipes_1](https://github.com/otto-link/HighMap/assets/121820229/4bb37a9e-e29f-43b9-b259-91bc16bfb536)


## Warning 

Work in progress, use at your own risk!

![WS310836__67171](https://github.com/otto-link/HighMap/assets/121820229/43dd1f39-06a3-4c32-955a-cf2609a58f17)

## License

This project is licensed under the GNU General Public License v3.0.

## Getting started

### Getting the sources

Use `git` to retrieve the sources: 
``` bash
git clone git@github.com:otto-link/HighMap.git
cd HighMap
git submodule update --init --recursive
```

### Building

#### Linux

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

#### Windows

Use for `PowerShell` to clone the repository (because Visual Studio won't clone the submodules) using `git` command lines (see above).

Install the missing OpenSource dependencies using `vcpkg`:
```
vcpkg install eigen3 libpng boost-range boost-type-erasure boost-geometry glm
```

You should then be able to build the sources using Visual Studio.

### Usage examples

ToDo.

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
