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
#include <cmath>

#include "highmap/array.hpp"

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
#define CD  {1.f, 1.f, 1.f, 1.f, M_SQRT2, M_SQRT2, M_SQRT2, M_SQRT2}
// clang-format on

namespace hmap
{

// void depression_breaching(Array &z);

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
 * @brief
 *
 * @param z_before Input array (before erosion).
 * @param z_after Input array (after erosion).
 * @param erosion_map Erosion map.
 * @param deposition_map Deposition map.
 * @param tolerance Tolerance for erosion / deposition definition.
 *
 * **Example**
 * @include ex_erosions_maps.cpp
 *
 * **Result**
 * @image html ex_erosions_maps0.png
 * @image html ex_erosions_maps1.png
 */
void erosion_maps(Array &z_before,
                  Array &z_after,
                  Array &erosion_map,
                  Array &deposition_map,
                  float  tolerance = 0.f);

/**
 * @brief Apply an algerbic formula based on the local gradient to perform
 * erosion/deposition.
 *
 * @param z Input array.
 * @param p_mask Intensity mask, expected in [0, 1] (applied as a
 * post-processing).
 * @param talus_ref Reference talus.
 * @param ir Smoothing prefilter radius.
 * @param p_bedrock Reference to the bedrock heightmap.
 * @param p_erosion_map[out] Reference to the erosion map, provided as an output
 * field.
 * @param p_deposition_map [out] Reference to the deposition map, provided as an
 * output field.
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
                        Array *p_mask,
                        float  talus_ref,
                        int    ir,
                        Array *p_bedrock = nullptr,
                        Array *p_erosion_map = nullptr,
                        Array *p_deposition_map = nullptr,
                        float  c_erosion = 0.05f,
                        float  c_deposition = 0.05f,
                        int    iterations = 1);

void hydraulic_algebric(Array &z,
                        float  talus_ref,
                        int    ir,
                        Array *p_bedrock = nullptr,
                        Array *p_erosion_map = nullptr,
                        Array *p_deposition_map = nullptr,
                        float  c_erosion = 0.05f,
                        float  c_deposition = 0.05f,
                        int    iterations = 1); ///< @overload

/**
 * @brief Apply cell-based hydraulic erosion/deposition based on Benes et al.
 * procedure.
 *
 * See @cite Benes2002 and @cite Olsen2004.
 *
 * @param z Input array.
 * @param p_mask Intensity mask, expected in [0, 1] (applied as a
 * post-processing).
 * @param iterations Number of iterations.
 * @param p_bedrock Reference to the bedrock heightmap.
 * @param p_moisture_map Reference to the moisture map (quantity of rain),
 * expected to be in [0, 1].
 * @param p_erosion_map[out] Reference to the erosion map, provided as an output
 * field.
 * @param p_deposition_map [out] Reference to the deposition map, provided as an
 * output field.
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
                     Array *p_mask,
                     int    iterations = 50,
                     Array *p_bedrock = nullptr,
                     Array *p_moisture_map = nullptr,
                     Array *p_erosion_map = nullptr,
                     Array *p_deposition_map = nullptr,
                     float  c_capacity = 40.f,
                     float  c_erosion = 0.2f,
                     float  c_deposition = 0.8f,
                     float  water_level = 0.005f,
                     float  evap_rate = 0.01f,
                     float  rain_rate = 0.5f);

void hydraulic_benes(Array &z,
                     int    iterations = 50,
                     Array *p_bedrock = nullptr,
                     Array *p_moisture_map = nullptr,
                     Array *p_erosion_map = nullptr,
                     Array *p_deposition_map = nullptr,
                     float  c_capacity = 40.f,
                     float  c_erosion = 0.2f,
                     float  c_deposition = 0.8f,
                     float  water_level = 0.005f,
                     float  evap_rate = 0.01f,
                     float  rain_rate = 0.5f); ///< @overload

/**
 * @brief Apply cell-based hydraulic erosion using a nonlinear diffusion model.
 * @param z Input array.
 * @param radius Gaussian filter radius (with respect to a unit domain).
 * @param vmax Maximum elevation for the details.
 * @param k_smoothing Smoothing factor, if any.
 *
 * **Example**
 * @include ex_hydraulic_blur.cpp
 *
 * **Result**
 * @image html ex_hydraulic_blur.png
 */
void hydraulic_blur(Array &z,
                    float  radius,
                    float  vmax,
                    float  k_smoothing = 0.1f);

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
 * @param p_mask Intensity mask, expected in [0, 1] (applied as a
 * post-processing).
 * @param nparticles Number of particles.
 * @param seed Random seed number.
 * @param p_bedrock Reference to the bedrock heightmap.
 * @param p_moisture_map Reference to the moisture map (quantity of rain),
 * expected to be in [0, 1].
 * @param p_erosion_map[out] Reference to the erosion map, provided as an output
 * field.
 * @param p_deposition_map [out] Reference to the deposition map, provided as an
 * output field.
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
 * @image html ex_hydraulic_particle0.png
 * @image html ex_hydraulic_particle1.png
 */
void hydraulic_particle(Array &z,
                        Array *p_mask,
                        int    nparticles,
                        int    seed,
                        Array *p_bedrock = nullptr,
                        Array *p_moisture_map = nullptr,
                        Array *p_erosion_map = nullptr,    // -> out
                        Array *p_deposition_map = nullptr, // -> out
                        int    c_radius = 0,
                        float  c_capacity = 10.f,
                        float  c_erosion = 0.05f,
                        float  c_deposition = 0.01f,
                        float  drag_rate = 0.01f,
                        float  evap_rate = 0.001f);

void hydraulic_particle(Array &z,
                        int    nparticles,
                        int    seed,
                        Array *p_bedrock = nullptr,
                        Array *p_moisture_map = nullptr,
                        Array *p_erosion_map = nullptr,    // -> out
                        Array *p_deposition_map = nullptr, // -> out
                        int    c_radius = 0,
                        float  c_capacity = 10.f,
                        float  c_erosion = 0.05f,
                        float  c_deposition = 0.01f,
                        float  drag_rate = 0.01f,
                        float  evap_rate = 0.001f); ///< @overload

/**
 * @brief Apply hydraulic erosion using a particle based procedure, using a
 * pyramid decomposition to allow a multiscale approach.
 * @param z Input array.
 * @param particle_density Particles density (with respect to the number of
 * cells of the input array).
 * @param seed Random seed number.
 * @param p_bedrock Reference to the bedrock heightmap.
 * @param p_moisture_map Reference to the moisture map (quantity of rain),
 * expected to be in [0, 1].
 * @param p_erosion_map[out] Reference to the erosion map, provided as an output
 * field.
 * @param p_deposition_map [out] Reference to the deposition map, provided as an
 * output field.
 * @param c_radius Particle radius in pixel(s) (>= 0).
 * @param c_capacity Sediment capacity.
 * @param c_deposition Deposition coefficient.
 * @param c_erosion Erosion coefficient.
 * @param drag_rate Drag rate.
 * @param evap_rate Particle evaporation rate.
 * @param pyramid_finest_level First level at which the erosion is applied
 * (default is 0, meaning it is applied to the current resolution, the 0th
 * pyramid level, and then to the coarser pyramid levels, if set to 1 it starts
 * with the first pyramid level and so on).
 *
 * **Example**
 * @include ex_hydraulic_particle_multiscale.cpp
 *
 * **Result**
 * @image html ex_hydraulic_particle_multiscale0.png
 * @image html ex_hydraulic_particle_multiscale1.png
 */
void hydraulic_particle_multiscale(Array &z,
                                   float  particle_density,
                                   int    seed,
                                   Array *p_bedrock = nullptr,
                                   Array *p_moisture_map = nullptr,
                                   Array *p_erosion_map = nullptr,    // -> out
                                   Array *p_deposition_map = nullptr, // -> out
                                   int    c_radius = 0,
                                   float  c_capacity = 10.f,
                                   float  c_erosion = 0.05f,
                                   float  c_deposition = 0.01f,
                                   float  drag_rate = 0.01f,
                                   float  evap_rate = 0.001f,
                                   int    pyramid_finest_level = 0);

/**
 * @brief Apply large-scale hydraulic erosion to produce "deep" ridges.
 *
 * @param z Input array.
 * @param talus Ridge talus.
 * @param intensity Erosion intensity in [0, 1].
 * @param p_mask Intensity mask, expected in [0, 1] (applied as a
 * post-processing).
 * @param erosion_factor Erosion factor, generally in ]0, 10]. Smaller values
 * tend to flatten the map.
 * @param smoothing_factor Smooothing factor in ]0, 1] (1 for no smoothing).
 * @param noise_ratio Ridge talus noise ratio in [0, 1].
 * @param ir Prefilter radius (in pixels).
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
                     int    ir = 0,
                     uint   seed = 1);

void hydraulic_ridge(Array &z,
                     float  talus,
                     Array *p_mask,
                     float  intensity = 0.5f,
                     float  erosion_factor = 1.5f,
                     float  smoothing_factor = 0.f,
                     float  noise_ratio = 0.f,
                     int    ir = 0,
                     uint   seed = 1); ///< @overload

/**
 * @brief Apply hydraulic erosion based on the Stream Power Law formulation.
 *
 * @param z Input array.
 * @param p_mask Intensity mask, expected in [0, 1] (applied as a
 * post-processing).
 * @param c_erosion Erosion coefficient.
 * @param talus_ref Reference talus used to localy define the flow-partition
 * exponent (small values of `talus_ref` will lead to thinner flow streams, see
 * {@link flow_accumulation_dinf}).
 * @param iterations Number of iterations.
 * @param p_bedrock Lower elevation limit.
 * @param p_moisture_map Reference to the moisture map (quantity of rain),
 * expected to be in [0, 1].
 * @param p_erosion_map[out] Reference to the erosion map, provided as an output
 * field.
 * @param ir Gradient prefiltering radius.
 *
 * **Example**
 * @include ex_hydraulic_spl.cpp
 *
 * **Result**
 * @image html ex_hydraulic_spl0.png
 * @image html ex_hydraulic_spl1.png
 */
void hydraulic_spl(Array &z,
                   float  c_erosion,
                   float  talus_ref,
                   int    iterations,
                   Array *p_bedrock = nullptr,
                   Array *p_moisture_map = nullptr,
                   Array *p_erosion_map = nullptr, // -> out
                   int    ir = 8);

void hydraulic_spl(Array &z,
                   Array *p_mask,
                   float  c_erosion,
                   float  talus_ref,
                   int    iterations,
                   Array *p_bedrock = nullptr,
                   Array *p_moisture_map = nullptr,
                   Array *p_erosion_map = nullptr, // -> out
                   int    ir = 8);                    ///< @overload

/**
 * @brief Apply hydraulic erosion based on a flow accumulation map.
 *
 * @param z Input array.
 * @param p_mask Intensity mask, expected in [0, 1] (applied as a
 * post-processing).
 * @param c_erosion Erosion coefficient.
 * @param talus_ref Reference talus used to localy define the flow-partition
 * exponent (small values of `talus_ref` will lead to thinner flow streams, see
 * {@link flow_accumulation_dinf}).
 * @param p_bedrock Lower elevation limit.
 * @param p_moisture_map Reference to the moisture map (quantity of rain),
 * expected to be in [0, 1].
 * @param p_erosion_map[out] Reference to the erosion map, provided as an output
 * field.
 * @param ir Kernel radius. If `ir > 1`, a cone kernel is used to carv channel
 * flow erosion.
 * @param clipping_ratio Flow accumulation clipping ratio.
 *
 * **Example**
 * @include ex_hydraulic_stream.cpp
 *
 * **Result**
 * @image html ex_hydraulic_stream0.png
 * @image html ex_hydraulic_stream1.png
 */
void hydraulic_stream(Array &z,
                      float  c_erosion,
                      float  talus_ref,
                      Array *p_bedrock = nullptr,
                      Array *p_moisture_map = nullptr,
                      Array *p_erosion_map = nullptr, // -> out
                      int    ir = 1,
                      float  clipping_ratio = 10.f);

void hydraulic_stream(Array &z,
                      Array *p_mask,
                      float  c_erosion,
                      float  talus_ref,
                      Array *p_bedrock = nullptr,
                      Array *p_moisture_map = nullptr,
                      Array *p_erosion_map = nullptr, // -> out
                      int    ir = 1,
                      float  clipping_ratio = 10.f); ///< @overload

/**
 * @brief Apply hydraulic erosion based on a flow accumulation map, alternative
 * formulation.
 *
 * @param z Input array.
 * @param p_mask Intensity mask, expected in [0, 1] (applied as a
 * post-processing).
 * @param c_erosion Erosion coefficient.
 * @param talus_ref Reference talus used to localy define the flow-partition
 * exponent (small values of `talus_ref` will lead to thinner flow streams, see
 * {@link flow_accumulation_dinf}).
 * @param gamma Gamma correction applied to the erosion.
 * @param p_bedrock Lower elevation limit.
 * @param p_moisture_map Reference to the moisture map (quantity of rain),
 * expected to be in [0, 1].
 * @param p_erosion_map[out] Reference to the erosion map, provided as an output
 * field.
 * @param ir Kernel radius. If `ir > 1`, a cone kernel is used to carv channel
 * flow erosion.
 *
 * **Example**
 * @include ex_hydraulic_stream.cpp
 *
 * **Result**
 * @image html ex_hydraulic_stream0.png
 * @image html ex_hydraulic_stream1.png
 */
void hydraulic_stream_log(Array &z,
                          float  c_erosion,
                          float  talus_ref,
                          float  gamma,
                          Array *p_bedrock = nullptr,
                          Array *p_moisture_map = nullptr,
                          Array *p_erosion_map = nullptr, // -> out
                          int    ir = 1);

void hydraulic_stream_log(Array &z,
                          Array *p_mask,
                          float  c_erosion,
                          float  talus_ref,
                          float  gamma,
                          Array *p_bedrock = nullptr,
                          Array *p_moisture_map = nullptr,
                          Array *p_erosion_map = nullptr, // -> out
                          int    ir = 1);                    ///< @overload

/**
 * @brief Apply hydraulic erosion using the 'virtual pipes' algorithm.
 *
 * See @cite Chiba1998, @cite Isheden2022, @cite Mei2007 and @cite Stava2008.
 *
 * @param z Input array.
 * @param p_mask Intensity mask, expected in [0, 1] (applied as a
 * @param iterations Number of iterations.
 * @param p_bedrock Lower elevation limit.
 * @param p_moisture_map Reference to the moisture map (quantity of rain),
 * expected to be in [0, 1].
 * @param p_erosion_map[out] Reference to the erosion map, provided as an output
 * field.
 * @param p_deposition_map [out] Reference to the deposition map, provided as an
 * output field.
 * @param water_height Water height.
 * @param c_capacity Sediment capacity.
 * @param c_deposition Deposition coefficient.
 * @param c_erosion Erosion coefficient.
 * @param rain_rate Rain rate.
 * @param evap_rate Particle evaporation rate.
 *
 * **Example**
 * @include ex_hydraulic_vpipes.cpp
 *
 * **Result**
 * @image html ex_hydraulic_vpipes.png
 */
void hydraulic_vpipes(Array &z,
                      Array *p_mask,
                      int    iterations,
                      Array *p_bedrock = nullptr,
                      Array *p_moisture_map = nullptr,
                      Array *p_erosion_map = nullptr,
                      Array *p_deposition_map = nullptr,
                      float  water_height = 0.1f,
                      float  c_capacity = 0.1f,
                      float  c_erosion = 0.05f,
                      float  c_deposition = 0.05f,
                      float  rain_rate = 0.f,
                      float  evap_rate = 0.01f);

void hydraulic_vpipes(Array &z,
                      int    iterations,
                      Array *p_bedrock = nullptr,
                      Array *p_moisture_map = nullptr,
                      Array *p_erosion_map = nullptr,
                      Array *p_deposition_map = nullptr,
                      float  water_height = 0.1f,
                      float  c_capacity = 0.1f,
                      float  c_erosion = 0.05f,
                      float  c_deposition = 0.05f,
                      float  rain_rate = 0.f,
                      float  evap_rate = 0.01f); ///< @overload

/**
 * @brief Perform sediment deposition combined with thermal erosion.
 *
 * @todo deposition map
 *
 * @param z Input array.
 * @param p_mask Intensity mask, expected in [0, 1] (applied as a
 * post-processing).
 * @param talus Talus limit.
 * @param p_deposition_map [out] Reference to the deposition map, provided as an
 * output field.
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
void sediment_deposition(Array       &z,
                         Array       *p_mask,
                         const Array &talus,
                         Array       *p_deposition_map = nullptr,
                         float        max_deposition = 0.01,
                         int          iterations = 5,
                         int          thermal_subiterations = 10);

void sediment_deposition(Array       &z,
                         const Array &talus,
                         Array       *p_deposition_map = nullptr,
                         float        max_deposition = 0.01,
                         int          iterations = 5,
                         int          thermal_subiterations = 10);

/**
 * @brief
 *
 * @param z Input array.
 * @param p_mask Intensity mask, expected in [0, 1] (applied as a
 * post-processing).
 * @param nparticles Number of particles.
 * @param ir Particle deposition radius.
 * @param seed Random seed number.
 * @param p_spawning_map Reference to the particle spawning density map.
 * @param p_deposition_map Reference to the deposition map (output).
 * @param particle_initial_sediment Initial sediment amount carried out by the
 * particles.
 * @param deposition_velocity_limit Deposition at which the deposition occurs.
 * @param drag_rate Particle drag rate.
 *
 * **Example**
 * @include ex_sediment_deposition_particle.cpp
 *
 * **Result**
 * @image html ex_sediment_deposition_particle.png
 */
void sediment_deposition_particle(Array &z,
                                  Array *p_mask,
                                  int    nparticles,
                                  int    ir,
                                  int    seed = 1,
                                  Array *p_spawning_map = nullptr,
                                  Array *p_deposition_map = nullptr,
                                  float  particle_initial_sediment = 0.1f,
                                  float  deposition_velocity_limit = 0.01f,
                                  float  drag_rate = 0.001f);

void sediment_deposition_particle(Array &z,
                                  int    nparticles,
                                  int    ir,
                                  int    seed = 1,
                                  Array *p_spawning_map = nullptr,
                                  Array *p_deposition_map = nullptr,
                                  float  particle_initial_sediment = 0.1f,
                                  float  deposition_velocity_limit = 0.01f,
                                  float  drag_rate = 0.001f);

/**
 * @brief Stratify the heightmap by creating a series of layers with elevations
 * corrected by a gamma factor.
 *
 * @param z Input array.
 * @param p_mask Intensity mask, expected in [0, 1] (applied as a
 * post-processing).
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
void stratify(Array             &z,
              Array             *p_mask,
              std::vector<float> hs,
              std::vector<float> gamma,
              Array             *p_noise = nullptr);

void stratify(Array             &z,
              std::vector<float> hs,
              std::vector<float> gamma,
              Array             *p_noise = nullptr); ///< @overload

void stratify(Array             &z,
              std::vector<float> hs,
              float              gamma = 0.5f,
              Array             *p_noise = nullptr); ///< @overload

/**
 * @brief Stratify the heightmap by creating a multiscale series of layers with
 * elevations corrected by a gamma factor.
 *
 * @param z Input array.
 * @param zmin Minimum elevation for the strata
 * @param zmax Maximum elevation for the strata
 * @param n_strata Number of strata for each stratification iteration.
 * @param strata_noise Elevation relative noise.
 * @param gamma_list Gamma value for each stratification iteration.
 * @param gamma_noise Gamma relative noise.
 * @param seed Random seed number.
 * @param p_mask Intensity mask, expected in [0, 1] (applied as a
 * post-processing).
 * @param p_noise Local elevation noise.
 *
 *
 * **Example**
 * @include ex_stratify_multiscale.cpp
 *
 * **Result**
 * @image html ex_stratify_multiscale.png
 */
void stratify_multiscale(Array             &z,
                         float              zmin,
                         float              zmax,
                         std::vector<int>   n_strata,
                         std::vector<float> strata_noise,
                         std::vector<float> gamma_list,
                         std::vector<float> gamma_noise,
                         uint               seed,
                         Array             *p_mask = nullptr,
                         Array             *p_noise = nullptr);

/**
 * @brief Stratify the heightmap by creating a series of oblique layers with
 * elevations corrected by a gamma factor.
 *
 * @param z Input array.
 * @param p_mask Intensity mask, expected in [0, 1] (applied as a
 * post-processing).
 * @param hs Layer elevations. For 'n' layers, 'n + 1' values must be provided.
 * @param gamma Layer gamma correction factors, 'n' values.
 * @param talus Layer talus value (slope).
 * @param angle Slope orientation (in degrees).
 * @param p_noise Local elevation noise.
 *
 * **Example**
 * @include ex_stratify.cpp
 *
 * **Result**
 * @image html ex_stratify.png
 */
void stratify_oblique(Array             &z,
                      Array             *p_mask,
                      std::vector<float> hs,
                      std::vector<float> gamma,
                      float              talus,
                      float              angle,
                      Array             *p_noise = nullptr);

void stratify_oblique(Array             &z,
                      std::vector<float> hs,
                      std::vector<float> gamma,
                      float              talus,
                      float              angle,
                      Array             *p_noise = nullptr); ///< @overload

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
 * @param p_mask Filter mask, expected in [0, 1].
 * @param talus Talus limit.
 * @param p_bedrock Lower elevation limit.
 * @param p_deposition_map [out] Reference to the deposition map, provided as an
 * output field.
 * @param iterations Number of iterations.
 *
 * **Example**
 * @include ex_thermal.cpp
 *
 * **Result**
 * @image html ex_thermal.png
 */
void thermal(Array       &z,
             Array       *p_mask,
             const Array &talus,
             int          iterations = 10,
             Array       *p_bedrock = nullptr,
             Array       *p_deposition_map = nullptr);

void thermal(Array       &z,
             const Array &talus,
             int          iterations = 10,
             Array       *p_bedrock = nullptr,
             Array       *p_deposition_map = nullptr);

void thermal(Array &z,
             float  talus,
             int    iterations = 10,
             Array *p_bedrock = nullptr,
             Array *p_deposition_map = nullptr); ///< @overload

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
 * @param p_deposition_map [out] Reference to the deposition map, provided as an
 * output field.
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
                          Array       *p_deposition_map = nullptr);

void thermal_auto_bedrock(Array &z,
                          float  talus,
                          int    iterations = 10,
                          Array *p_deposition_map = nullptr); ///< @overload

void thermal_auto_bedrock(Array &z,
                          Array *p_mask,
                          float  talus,
                          int    iterations = 10,
                          Array *p_deposition_map = nullptr); ///< @overload

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
 * @brief Apply thermal erosion using a 'rib' algorithm (taken from Geomorph).
 * @param z Input heightmap.
 * @param iterations Number of iterations.
 * @param p_bedrock Lower elevation limit.
 *
 * **Example**
 * @include ex_thermal_rib.cpp
 *
 * **Result**
 * @image html ex_thermal_rib.png
 */
void thermal_rib(Array &z, int iterations, Array *p_bedrock = nullptr);

/**
 * @brief Apply thermal weathering erosion simulating scree deposition.
 *
 * @param z Input array.
 * @param p_mask Filter mask, expected in [0, 1].
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
                   Array *p_mask,
                   float  talus,
                   uint   seed,
                   float  zmax,
                   float  zmin,
                   float  noise_ratio,
                   Array *p_deposition_map = nullptr,
                   float  landing_talus_ratio = 1.f,
                   float  landing_width_ratio = 0.25f,
                   bool   talus_constraint = true);

void thermal_scree(Array &z,
                   float  talus,
                   uint   seed,
                   float  zmax,
                   float  zmin,
                   float  noise_ratio,
                   Array *p_deposition_map = nullptr,
                   float  landing_talus_ratio = 1.f,
                   float  landing_width_ratio = 0.25f,
                   bool   talus_constraint = true); ///< @overload

void thermal_scree(Array &z,
                   float  talus,
                   uint   seed,
                   float  zmax,
                   float  noise_ratio,
                   Array *p_deposition_map = nullptr); ///< @overload

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
void thermal_scree_fast(Array    &z,
                        Vec2<int> shape_coarse,
                        float     talus,
                        uint      seed,
                        float     zmax,
                        float     zmin,
                        float     noise_ratio,
                        float     landing_talus_ratio,
                        float     landing_width_ratio,
                        bool      talus_constraint);

void thermal_scree_fast(Array    &z,
                        Vec2<int> shape_coarse,
                        float     talus,
                        uint      seed,
                        float     zmax,
                        float     noise_ratio); ///< @overload

} // namespace hmap
