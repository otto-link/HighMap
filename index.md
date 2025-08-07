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

### "Array" - Elementary Data Structure for Building Heightmaps

The `Array` object is the fundamental class upon which the heightmap
construction algorithms rely. A heightmap is a two-dimensional
representation where each cell's value corresponds to a height or
elevation at that point. An `Array` is simply a 2D array with
dimensions defined by `shape` (length/width).

Indexing is typically based on a pair of indices `(i, j)`. The cell at
`(i=0, j=0)` is assumed to be at the bottom-left corner of the
heightmap. This assumption is usually inconsequential, except when the
heightmap needs to be aligned with specific spatial positions `(x,
y)`. The conversion between indices `(i, j)` and spatial positions `(x, y)`
depends on the heightmap's "bounding box," which is the spatial domain
covered by the heightmap. By default, this bounding box is assumed to
be a unit domain, meaning it spans a square of size 1 in both x and y
directions.

The values within the heightmap cells are generally within the range
`[0, 1]`. While this range is not strictly enforced, some algorithms
assume that the data adheres to this scale. It is always possible to
use the \ref hmap::remap function to adjust the scale of the heightmap
values to fit within the desired range.

### Example usage

#### Generate classical coherent noises (Perlin, Simplex, fractal layering, etc...)

#### Filters

#### Physical processes

#### Bivariate functions

### Exporting

#### Image formats (png, tiff, exr...)

#### Numpy binary file

The library includes a function that enables exporting data to the
NumPy binary format (.npy), facilitating integration with Python's
NumPy library.

@include ex_to_numpy.cpp

#### Raw binary file

#### Asset export (3D mesh and texture)

hmap::AssetExportFormat

hmap::MeshType

## Library functionalities

| Header | Description |
|--|--|
| array.hpp | Declaration of the Array class for 2D floating-point arrays with various mathematical operations and utilities. |