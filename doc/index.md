\mainpage

# HighMap library documentation!

Welcome to the documentation of the HighMap, a C++ library designed
to facilitate the generation of heightmaps for video games,
simulations, and other applications that require detailed terrain
data.

## Heightmaps?

A heightmap is a type of digital image or data grid that represents
the elevation or height of terrain in a two-dimensional format. Each
pixel or point in a heightmap corresponds to a specific location on
the terrain, with the value at that point indicating the elevation
above anny reference level.

In a grayscale heightmap, darker shades typically represent lower
elevations, while lighter shades indicate higher elevations. This
visual representation allows developers to easily map out and
manipulate terrain features such as hills, valleys, mountains, and
plains in a virtual environment.

![](images/illustrations/dem_example_rendered.png "Heightmap (left) and 3D rendering of this heightmap (right).")

## Getting started

### Sources and installation

For installation instructions, please refer to the README in the
GitHub repository:
[https://github.com/otto-link/HighMap](https://github.com/otto-link/HighMap).

### Basic principle and data structure

The library represents heightmaps using a 2D array of floating-point
values, where each value corresponds to the elevation at a specific
point on the terrain. To work with these heightmaps, the library
includes specialized data structures that allow for efficient
generation, manipulation, and modification of these arrays, making it
easier to create and adjust complex terrain features.

Provided data structures are:

- 2D array,

  - \ref hmap.Array for 2D array of floats,

  - \ref hmap.HeightMap for a tiled representation and parallel
    computation of 2D array of floats,
  
- bivariate \f((x, y)\f) functions for locally computable noise
  generators or basic functions,

  - \ref hmap.Function and its derivates \ref hmap.NoiseFunction, \ref
    hmap.FbmFunction, \ref hmap.FieldFunction,...

- geometric primitives,

  - \ref hmap.Point,
  
  - \ref hmap.Edge,

  - \ref hmap.Cloud for set of points ,

  - \ref hmap.Path for set of ordered points (i.e. open or closed polylines),

  - \ref hmap.Graph.

## Usage examples

Simple usage example are provided for most of the functions in the
repertory `examples` located at the root of the repository
[HighMap](https://github.com/otto-link/HighMap).

### Generate classical coherent noises (Perlin, Simplex, fractal layering, etc...)

### Filters

### Physical processes

### Bivariate functions

## Library functionalities

| Header | Description |
|--|--|
| array.hpp | Declaration of the Array class for 2D floating-point arrays with various mathematical operations and utilities. |