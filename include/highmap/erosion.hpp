/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file erosion.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

// neighbor pattern search based on Moore pattern and define diagonal
// weight coefficients ('c' corresponds to a weight coefficient
// applied to take into account the longer distance for diagonal
// comparison between cells)

// 6 2 8
// 1 . 4
// 5 3 7
// clang-format off
#define DI {-1, 0, 0, 1, -1, -1, 1, 1}
#define DJ {0, 1, -1, 0, -1, 1, -1, 1}
#define C  {1.f, 1.f, 1.f, 1.f, M_SQRT2, M_SQRT2, M_SQRT2, M_SQRT2}
// clang-format on

namespace hmap
{

/**
 * @brief Fill the depressions of the heightmap using the Planchon-Darboux
 * algorithm.
 *
 * Fill heightmap depressions to ensure that every cell can be connected to the
 * boundaries following a downward slope @cite Planchon2002.
 *
 * @param z Input array.
 * @param iterations Number of iterations.
 * @param epsilon
 *
 * **Example**
 * @include ex_depression_filling.cpp
 *
 * **Result**
 * @image html ex_depression_filling.png
 */
void depression_filling(Array &z, int iterations = 1000, float epsilon = 1e-4f);

/**
 * @brief Apply an algerbic formula based on the local gradient to perform
 * erosion/deposition.
 *
 * @param z Input array.
 * @param talus_ref Reference talus.
 * @param ir Smoothing prefilter radius.
 * @param c_erosion Erosion coefficient.
 * @param c_deposition Deposition coefficient.
 * @param iterations Number of iterations.
 *
 * **Example**
 * @include ex_hydraulic_algebric.cpp
 *
 * **Result**
 * @image html ex_hydraulic_algebric.png
 */
void hydraulic_algebric(Array &z,
                        float  talus_ref,
                        int    ir,
                        float  c_erosion = 0.05f,
                        float  c_deposition = 0.05f,
                        int    iterations = 1);

/**
 * @brief Apply cell-based hydraulic erosion/deposition based on Benes et al.
 * procedure.
 *
 * See @cite Benes2002 and @cite Olsen2004.
 *
 * @param z Input array.
 * @param rain_map Moisture map (quantity of rain), expected to be in [0, 1].
 * @param iterations Number of iterations.
 * @param c_capacity Sediment capacity.
 * @param c_deposition Deposition coefficient.
 * @param c_erosion Erosion coefficient.
 * @param water_level Water level.
 * @param evap_rate Water evaporation rate.
 * @param rain_rate Rain relaxation rate.
 *
 * **Example**
 * @include ex_hydraulic_benes.cpp
 *
 * **Result**
 * @image html ex_hydraulic_benes.png
 */
void hydraulic_benes(Array &z,
                     Array &rain_map,
                     int    iterations = 50,
                     float  c_capacity = 40.f,
                     float  c_erosion = 0.2f,
                     float  c_deposition = 0.8f,
                     float  water_level = 0.005f,
                     float  evap_rate = 0.01f,
                     float  rain_rate = 0.5f);

void hydraulic_benes(Array &z,
                     int    iterations = 50,
                     float  c_capacity = 40.f,
                     float  c_erosion = 0.2f,
                     float  c_deposition = 0.8f,
                     float  water_level = 0.005f,
                     float  evap_rate = 0.01f,
                     float  rain_rate = 0.5f); ///< @overload

/**
 * @brief Apply cell-based hydraulic erosion using a nonlinear diffusion model.
 *
 * See @cite Roering2001.
 *
 * @param z Input array.
 * @param c_diffusion Diffusion coefficient.
 * @param talus Reference talus (must be higher than the maximum talus of the
 * map).
 * @param iterations Number of iterations.
 *
 * **Example**
 * @include ex_hydraulic_diffusion.cpp
 *
 * **Result**
 * @image html ex_hydraulic_diffusion.png
 */
void hydraulic_diffusion(Array &z,
                         float  c_diffusion,
                         float  talus,
                         int    iterations);
/**
 * @brief Apply cell-based hydraulic erosion/deposition of Musgrave et al.
 * (1989).
 *
 * A simple grid-based erosion technique was published by Musgrave, Kolb,
 * and Mace in 1989 @cite Musgrave1989.
 *
 * @param z Input array.
 * @param moisture_map Moisture map (quantity of rain), expected to be in
 * [0, 1].
 * @param iterations Number of iterations.
 * @param c_capacity Sediment capacity.
 * @param c_deposition Deposition coefficient.
 * @param c_erosion Erosion coefficient.
 * @param water_level Water level.
 * @param evap_rate Water evaporation rate.
 *
 * **Example**
 * @include ex_hydraulic_musgrave.cpp
 *
 * **Result**
 * @image html ex_hydraulic_musgrave.png
 */
void hydraulic_musgrave(Array &z,
                        Array &moisture_map,
                        int    iterations = 100,
                        float  c_capacity = 1.f,
                        float  c_erosion = 0.1f,
                        float  c_deposition = 0.1f,
                        float  water_level = 0.01f,
                        float  evap_rate = 0.01f);

void hydraulic_musgrave(Array &z,
                        int    iterations = 100,
                        float  c_capacity = 1.f,
                        float  c_erosion = 0.1f,
                        float  c_deposition = 0.1f,
                        float  water_level = 0.01f,
                        float  evap_rate = 0.01f); ///< @overload

/**
 * @brief Apply hydraulic erosion using a particle based procedure.
 *
 * Adapted from @cite Beyer2015 and @cite Hjulstroem1935.
 *
 * @param z Input array.
 * @param moisture_map Moisture map (quantity of rain), expected to be in [0,
 * 1].
 * @param seed Random seed number.
 * @param nparticles Number of particles.
 * @param c_radius Particle radius in pixel(s) (>= 0).
 * @param c_capacity Sediment capacity.
 * @param c_deposition Deposition coefficient.
 * @param c_erosion Erosion coefficient.
 * @param drag_rate Drag rate.
 * @param evap_rate Particle evaporation rate.
 *
 * **Example**
 * @include ex_hydraulic_particle.cpp
 *
 * **Result**
 * @image html ex_hydraulic_particle.png
 */
void hydraulic_particle(Array &z,
                        Array &moisture_map,
                        int    nparticles,
                        int    seed,
                        int    c_radius = 0,
                        float  c_capacity = 10.f,
                        float  c_erosion = 0.05f,
                        float  c_deposition = 0.01f,
                        float  drag_rate = 0.01f,
                        float  evap_rate = 0.001f);

void hydraulic_particle(Array &z,
                        int    nparticles,
                        int    seed,
                        int    c_radius = 0,
                        float  c_capacity = 10.f,
                        float  c_erosion = 0.05f,
                        float  c_deposition = 0.01f,
                        float  drag_rate = 0.01f,
                        float  evap_rate = 0.001f); ///< @overload

/**
 * @brief Apply multiscale hydraulic erosion using a particle based procedure.
 *
 * @param z Input array.
 * @param moisture_map Moisture map (quantity of rain), expected to be in [0,
 * 1].
 * @param particle_density The number of particles simulated equals the particle
 * density multiplied by the heightmap size = shape[0]*shape[1].
 * @param scales List of scales, for instance, with `{16.f, 8.f, 4.f, 2.f}`,
 * hydraulic erosion is performed at scales 1/16th, 1/8th, ...
 * @param seed Random seed number.
 * @param c_radius Particle radius in pixel(s) (>= 0).
 * @param c_capacity Sediment capacity.
 * @param c_deposition Deposition coefficient.
 * @param c_erosion Erosion coefficient.
 * @param drag_rate Drag rate.
 * @param evap_rate Particle evaporation rate.
 *
 * **Example**
 * @include ex_hydraulic_particle_multiscale.cpp
 *
 * **Result**
 * @image html ex_hydraulic_particle_multiscale.png
 */
void hydraulic_particle_multiscale(Array             &z,
                                   Array             &moisture_map,
                                   float              particle_density,
                                   std::vector<float> scales,
                                   int                seed,
                                   int                c_radius = 0,
                                   float              c_capacity = 20.f,
                                   float              c_erosion = 0.05f,
                                   float              c_deposition = 0.01f,
                                   float              drag_rate = 0.01f,
                                   float              evap_rate = 0.001f);

/**
 * @brief Apply large-scale hydraulic erosion to produce "deep" ridges.
 *
 * @param z Input array.
 * @param talus Ridge talus.
 * @param intensity Erosion intensity in [0, 1].
 * @param erosion_factor Erosion factor, generally in ]0, 10]. Smaller values
 * tend to flatten the map.
 * @param smoothing_factor Smooothing factor in ]0, 1] (1 for no smoothing).
 * @param noise_ratio Ridge talus noise ratio in [0, 1].
 * @param seed Random seed number (only useful when `noise_ratio != 0`).
 *
 * **Example**
 * @include ex_hydraulic_ridge.cpp
 *
 * **Result**
 * @image html ex_hydraulic_ridge.png
 */
void hydraulic_ridge(Array &z,
                     float  talus,
                     float  intensity = 0.5f,
                     float  erosion_factor = 1.5f,
                     float  smoothing_factor = 0.f,
                     float  noise_ratio = 0.f,
                     uint   seed = 1);

/**
 * @brief Apply hydraulic erosion using based on a flow accumulation map.
 *
 * @param z Input array.
 * @param z_bedrock Lower elevation limit.
 * @param c_erosion Erosion coefficient.
 * @param talus_ref Reference talus used to localy define the flow-partition
 * exponent (small values of `talus_ref` will lead to thinner flow streams, see
 * {@link flow_accumulation_dinf}).
 * @param ir Kernel radius. If `ir > 1`, a cone kernel is used to carv channel
 * flow erosion.
 * @param clipping_ratio Flow accumulation clipping ratio.
 *
 * **Example**
 * @include ex_hydraulic_stream.cpp
 *
 * **Result**
 * @image html ex_hydraulic_stream.png
 */
void hydraulic_stream(Array &z,
                      Array &z_bedrock,
                      float  c_erosion,
                      float  talus_ref,
                      int    ir = 1,
                      float  clipping_ratio = 10.f);

void hydraulic_vpipes(Array &z);

/**
 * @brief Perform sediment deposition combined with thermal erosion.
 *
 * @todo deposition map
 *
 * @param z Input array.
 * @param talus Talus limit.
 * @param max_deposition Maximum height of sediment deposition.
 * @param iterations Number of iterations.
 * @param thermal_erosion_subiterations Number of thermal erosion iterations for
 * each pass.
 *
 * **Example**
 * @include ex_sediment_deposition.cpp
 *
 * **Result**
 * @image html ex_sediment_deposition.png
 */
void sediment_deposition(Array &z,
                         Array &talus,
                         float  max_deposition = 0.01,
                         int    iterations = 5,
                         int    thermal_subiterations = 10);

/**
 * @brief Stratify the heightmap by creating a series of layers with elevations
 * corrected by a gamma factor.
 *
 * @param z Input array.
 * @param hs Layer elevations. For 'n' layers, 'n + 1' values must be provided.
 * @param gamma Layer gamma correction factors, 'n' values.
 *
 * @see gamma_correction.
 *
 * **Example**
 * @include ex_stratify.cpp
 *
 * **Result**
 * @image html ex_stratify.png
 */
void stratify(Array &z, std::vector<float> hs, std::vector<float> gamma);

void stratify(Array             &z,
              std::vector<float> hs,
              float              gamma = 0.5f); ///< @overload

/**
 * @brief Apply thermal weathering erosion.
 *
 * Based on averaging over first neighbors, see \cite Olsen2004. Refer to \cite
 * Musgrave1989 for the original formulation.
 *
 * @todo optimize memory usage (to avoid large constant arrays).
 *
 * Thermal erosion refers to the process in which surface sediment weakens
 * due to temperature and detaches, falling down the slopes of the terrain
 * until a resting place is reached, where smooth plateaus tend to form
 * @cite Musgrave1989.
 *
 * @param z Input array.
 * @param talus Talus limit.
 * @param bedrock Lower elevation limit.
 * @param iterations Number of iterations.
 * @param ct "Avalanching" intensity (in [0, 1]).
 *
 * **Example**
 * @include ex_thermal.cpp
 *
 * **Result**
 * @image html ex_thermal.png
 */
void thermal(Array       &z,
             const Array &talus,
             const Array &bedrock,
             int          iterations = 10,
             float        ct = 0.5);

void thermal(Array       &array,
             const Array &talus,
             int          iterations = 10,
             float        ct = 0.5); ///< @overload

void thermal(Array &z,
             float  talus,
             int    iterations = 10,
             float  ct = 0.5); ///< @overload

/**
 * @brief Apply thermal weathering erosion with automatic determination of the
 * bedrock.
 *
 * @todo more comprehensive documentation on auto-bedrock algo.
 * @todo fix hard-coded parameters.
 *
 * @see {@link thermal}
 *
 * @param z Input array.
 * @param talus Local talus limit.
 * @param iterations Number of iterations.
 * @param ct "Avalanching" intensity (in [0, 1]).
 *
 * **Example**
 * @include ex_thermal_auto_bedrock.cpp
 *
 * **Result**
 * @image html ex_thermal_auto_bedrock.png
 */
void thermal_auto_bedrock(Array       &z,
                          const Array &talus,
                          int          iterations = 10,
                          float        ct = 0.5);

void thermal_auto_bedrock(Array &z,
                          float  talus,
                          int    iterations = 10,
                          float  ct = 0.5); ///< @overload

/**
 * @brief Apply modified thermal weathering of Olsen.
 *
 * Based on the algorithm of Olsen \cite Olsen2004, which "causes slopes steeper
 * than the talus threshold to remain unaffected while flatter areas are
 * levelled out".
 *
 * @param z Input array.
 * @param talus Local talus limit.
 * @param iterations Number of iterations.
 *
 * **Example**
 * @include ex_thermal_flatten.cpp
 *
 * **Result**
 * @image html ex_thermal_flatten.png
 */
void thermal_flatten(Array &z, const Array &talus, int iterations = 10);

void thermal_flatten(Array &z, float talus, int iterations = 10); ///< @overload

/**
 * @brief Apply thermal weathering erosion simulating scree deposition.
 *
 * @param z Input array.
 * @param talus Talus limit.
 * @param seed Random seed number.
 * @param zmax Elevation upper limit.
 * @param zmin Elevation lower limit.
 * @param noise_ratio Noise amplitude ratio (for talus and elevation limit).
 * @param landing_talus_ratio Talus value (as a ratio) at the scree landing (set
 * to 1 for no effect).
 * @param landing_width_ratio Landing relative extent, in [0, 1] (small values
 * lead to large landing).
 * @param talus_constraint Use talus constraint when populating the initial
 * queue.
 *
 * **Example**
 * @include ex_thermal_scree.cpp
 *
 * **Result**
 * @image html ex_thermal_scree.png
 */
void thermal_scree(Array &z,
                   float  talus,
                   uint   seed,
                   float  zmax,
                   float  zmin,
                   float  noise_ratio,
                   float  landing_talus_ratio = 1.f,
                   float  landing_width_ratio = 0.25f,
                   bool   talus_constraint = true);

void thermal_scree(Array &z,
                   float  talus,
                   uint   seed,
                   float  zmax,
                   float  noise_ratio); ///< @overload

/**
 * @brief Apply thermal weathering erosion simulating scree deposition,
 * performed on a coarse mesh to optimize restitution time.
 *
 * @param z Input array.
 * @param shape_coarse Array coarser shape used for the solver.
 * @param talus Talus limit.
 * @param seed Random seed number.
 * @param zmax Elevation upper limit.
 * @param zmin Elevation lower limit.
 * @param noise_ratio Noise amplitude ratio (for talus and elevation limit).
 * @param landing_talus_ratio Talus value (as a ratio) at the scree landing (set
 * to 1 for no effect).
 * @param landing_width_ratio Landing relative extent, in [0, 1] (small values
 * lead to large landing).
 * @param talus_constraint Use talus constraint when populating the initial
 * queue.
 *
 * **Example**
 * @include ex_thermal_scree.cpp
 *
 * **Result**
 * @image html ex_thermal_scree.png
 */
void thermal_scree_fast(Array           &z,
                        std::vector<int> shape_coarse,
                        float            talus,
                        uint             seed,
                        float            zmax,
                        float            zmin,
                        float            noise_ratio,
                        float            landing_talus_ratio,
                        float            landing_width_ratio,
                        bool             talus_constraint);

void thermal_scree_fast(Array           &z,
                        std::vector<int> shape_coarse,
                        float            talus,
                        uint             seed,
                        float            zmax,
                        float            noise_ratio); ///< @overload

} // namespace hmap
