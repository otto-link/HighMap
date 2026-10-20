# Core Concepts

This page describes the primary data structures and representations used in HighMap.

## 1. Elevation Representations

HighMap provides three main structures for 2D height data:

| Structure | Type | Description |
| :--- | :--- | :--- |
| `Array` | `hmap::Array` | In-memory 2D array of floats with contiguous storage. |
| `HeightMap` | `hmap::HeightMap` | Tiled 2D array designed for parallel computation. |
| `VirtualArray` | `hmap::VirtualArray` | Tiled structure supporting disk and RAM storage for large grids. |

### Array

`Array` is the basic container for 2D elevation fields. Data is stored in row-major order. It supports arithmetic operators, statistical queries, slicing, and filtering.

```cpp
hmap::Array z({512, 512});
z = 1.0f;
float val = z(i, j);
```

## 2. Bivariate Functions

Continuous functions deriving from `hmap::Function` represent mathematical surfaces $f(x, y)$ independently of resolution:

- `hmap::NoiseFunction`: Perlin, Simplex, and cellular noise generators.
- `hmap::FbmFunction`: Multi-octave fractal noise.
- `hmap::FieldFunction`: Distance fields and geometric functions.

## 3. Geometric Primitives

Geometric entities represent vector features and integrate with height arrays:

- `hmap::Point` / `hmap::Cloud`: Individual points and point clouds.
- `hmap::Edge` / `hmap::Path`: Segments and ordered sequences of points.
- `hmap::Graph`: Node and edge graphs used for networks such as rivers and roads.

## 4. Scattering

The scattering system handles the distribution of discrete objects over a terrain:

- `hmap::ScatterItem`: Basic element storing a 3D position, an integer class identifier, and a radius.
- `hmap::ScatterField`: Container providing spatial operations (collision pruning, density maps, elevation projection).
- `hmap::Forest`: Scattering container for vegetation and growth models.
- `hmap::RockField`: Scattering container for rock and debris placement.
