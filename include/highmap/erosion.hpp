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
 * @brief Apply cell-based hydraulic erosion/deposition of Musgrave et al.
 * (1989).
 *
 * A simple grid-based erosion technique was published by Musgrave, Kolb, and
 * Mace in 1989 @cite Musgrave1989.
 *
 * @param z Input array.
 * @param moisture_map Moisture map (quantity of rain), expected to be in [0,
 * 1].
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
                        float  c_capacity = 20.f,
                        float  c_erosion = 0.05f,
                        float  c_deposition = 0.01f,
                        float  drag_rate = 0.001f,
                        float  evap_rate = 0.001f);

void hydraulic_particle(Array &z,
                        int    nparticles,
                        int    seed,
                        int    c_radius = 0,
                        float  c_capacity = 20.f,
                        float  c_erosion = 0.05f,
                        float  c_deposition = 0.01f,
                        float  drag_rate = 0.001f,
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
                                   float              drag_rate = 0.001f,
                                   float              evap_rate = 0.001f);

/**
 * @brief Apply hydraulic erosion using based on a flow accumulation map.
 *
 * @param z Input array.
 * @param z_bedrock Lower elevation limit.
 * @param c_erosion Erosion coefficient.
 * @param talus_ref Reference talus used to localy define the flow-partition
 * exponent (see {@link flow_accumulation_dinf}).
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
                      float  clipping_ratio = 10.f);

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
void thermal(Array &z,
             Array &talus,
             Array &bedrock,
             int    iterations = 50,
             float  ct = 0.4,
             bool   post_filtering = true);

void thermal(Array &array,
             Array &talus,
             int    iterations = 50,
             float  ct = 0.4,
             bool   post_filtering = true); ///< @overload

void thermal(Array &z,
             float  talus,
             int    iterations = 50,
             float  ct = 0.4,
             bool   post_filtering = true); ///< @overload

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
void thermal_auto_bedrock(Array &z,
                          Array &talus,
                          int    iterations = 50,
                          float  ct = 0.4,
                          bool   post_filtering = true);

void thermal_auto_bedrock(Array &z,
                          float  talus,
                          int    iterations = 50,
                          float  ct = 0.4,
                          bool   post_filtering = true); ///< @overload

} // namespace hmap