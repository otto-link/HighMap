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
 * @image html ex_depression_filling0.png
 * @image html ex_depression_filling1.png
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
 * @image html ex_hydraulic_musgrave0.png
 * @image html ex_hydraulic_musgrave1.png
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
 * @image html ex_thermal0.png
 * @image html ex_thermal1.png
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
 * @image html ex_stratify0.png
 * @image html ex_stratify1.png
 */
void stratify(Array &z, std::vector<float> hs, std::vector<float> gamma);
void stratify(Array             &z,
              std::vector<float> hs,
              float              gamma = 0.5f); ///< @overload

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
 * @image html ex_thermal_auto_bedrock0.png
 * @image html ex_thermal_auto_bedrock1.png
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