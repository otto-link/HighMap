/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file erosion.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Implements hydraulic and thermal erosion algorithms for terrain
 * modeling, including particle-based flow, sediment transport (Musgrave, Benes,
 * Olsen), slope-driven diffusion, and procedural ridge formation. Supports
 * GPU-accelerated methods and generates erosion/deposition maps for geomorphic
 * analysis.
 *
 * @copyright Copyright (c) 2023 Otto Link.
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

/**
 * @brief Procedural erosion angular profile type.
 */
enum ErosionProfile : int
{
  COSINE,
  SAW_SHARP,
  SAW_SMOOTH,
  SHARP_VALLEYS,
  SQUARE_SMOOTH,
  TRIANGLE_GRENIER,
  TRIANGLE_SHARP,
  TRIANGLE_SMOOTH,
};

/**
 * @brief Fill the depressions of the heightmap using the Planchon-Darboux
 * algorithm.
 *
 * Fill heightmap depressions to ensure that every cell can be connected to the
 * boundaries following a downward slope @cite Planchon2002.
 *
 * @param z          Input array.
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
 * @param z_before       Input array (before erosion).
 * @param z_after        Input array (after erosion).
 * @param erosion_map    Erosion map.
 * @param deposition_map Deposition map.
 * @param tolerance      Tolerance for erosion / deposition definition.
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
 * @param z                  Input array.
 * @param p_mask             Intensity mask, expected in [0, 1] (applied as a
 *                           post-processing).
 * @param talus_ref          Reference talus.
 * @param ir                 Smoothing prefilter radius.
 * @param p_bedrock          Reference to the bedrock heightmap.
 * @param p_erosion_map[out] Reference to the erosion map, provided as an output
 *                           field.
 * @param p_deposition_map   [out] Reference to the deposition map, provided as
 *                           an output field.
 * @param c_erosion          Erosion coefficient.
 * @param c_deposition       Deposition coefficient.
 * @param iterations         Number of iterations.
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
 * @param z                  Input array.
 * @param p_mask             Intensity mask, expected in [0, 1] (applied as a
 *                           post-processing).
 * @param iterations         Number of iterations.
 * @param p_bedrock          Reference to the bedrock heightmap.
 * @param p_moisture_map     Reference to the moisture map (quantity of rain),
 *                           expected to be in [0, 1].
 * @param p_erosion_map[out] Reference to the erosion map, provided as an output
 *                           field.
 * @param p_deposition_map   [out] Reference to the deposition map, provided as
 *                           an output field.
 * @param c_capacity         Sediment capacity.
 * @param c_deposition       Deposition coefficient.
 * @param c_erosion          Erosion coefficient.
 * @param water_level        Water level.
 * @param evap_rate          Water evaporation rate.
 * @param rain_rate          Rain relaxation rate.
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
 * @param z           Input array.
 * @param radius      Gaussian filter radius (with respect to a unit domain).
 * @param vmax        Maximum elevation for the details.
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
 * @param z           Input array.
 * @param c_diffusion Diffusion coefficient.
 * @param talus       Reference talus (must be higher than the maximum talus of
 *                    the map).
 * @param iterations  Number of iterations.
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
 * A simple grid-based erosion technique was published by Musgrave, Kolb, and
 * Mace in 1989 @cite Musgrave1989.
 *
 * @param z            Input array.
 * @param moisture_map Moisture map (quantity of rain), expected to be in [0,
 *                     1].
 * @param iterations   Number of iterations.
 * @param c_capacity   Sediment capacity.
 * @param c_deposition Deposition coefficient.
 * @param c_erosion    Erosion coefficient.
 * @param water_level  Water level.
 * @param evap_rate    Water evaporation rate.
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
 * @param z                  Input array.
 * @param p_mask             Intensity mask, expected in [0, 1] (applied as a
 *                           post-processing).
 * @param nparticles         Number of particles.
 * @param seed               Random seed number.
 * @param p_bedrock          Reference to the bedrock heightmap.
 * @param p_moisture_map     Reference to the moisture map (quantity of rain),
 *                           expected to be in [0, 1].
 * @param p_erosion_map[out] Reference to the erosion map, provided as an output
 *                           field.
 * @param p_deposition_map   [out] Reference to the deposition map, provided as
 *                           an output field.
 * @param c_capacity         Sediment capacity.
 * @param c_deposition       Deposition coefficient.
 * @param c_erosion          Erosion coefficient.
 * @param drag_rate          Drag rate.
 * @param evap_rate          Particle evaporation rate.
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
                        float  c_capacity = 10.f,
                        float  c_erosion = 0.05f,
                        float  c_deposition = 0.05f,
                        float  c_inertia = 0.3f,
                        float  drag_rate = 0.001f,
                        float  evap_rate = 0.001f,
                        bool   post_filtering = false);

void hydraulic_particle(Array &z,
                        int    nparticles,
                        int    seed,
                        Array *p_bedrock = nullptr,
                        Array *p_moisture_map = nullptr,
                        Array *p_erosion_map = nullptr,    // -> out
                        Array *p_deposition_map = nullptr, // -> out
                        float  c_capacity = 10.f,
                        float  c_erosion = 0.05f,
                        float  c_deposition = 0.05f,
                        float  c_inertia = 0.3f,
                        float  drag_rate = 0.001f,
                        float  evap_rate = 0.001f,
                        bool   post_filtering = false); ///< @overload

/**
 * @brief Apply hydraulic erosion using a particle based procedure, using a
 * pyramid decomposition to allow a multiscale approach.
 * @param z                    Input array.
 * @param particle_density     Particles density (with respect to the number of
 *                             cells of the input array).
 * @param seed                 Random seed number.
 * @param p_bedrock            Reference to the bedrock heightmap.
 * @param p_moisture_map       Reference to the moisture map (quantity of rain),
 *                             expected to be in [0, 1].
 * @param p_erosion_map[out]   Reference to the erosion map, provided as an
 *                             output field.
 * @param p_deposition_map     [out] Reference to the deposition map, provided
 *                             as an output field.
 * @param c_capacity           Sediment capacity.
 * @param c_deposition         Deposition coefficient.
 * @param c_erosion            Erosion coefficient.
 * @param drag_rate            Drag rate.
 * @param evap_rate            Particle evaporation rate.
 * @param pyramid_finest_level First level at which the erosion is applied
 *                             (default is 0, meaning it is applied to the
 *                             current resolution, the 0th pyramid level, and
 *                             then to the coarser pyramid levels, if set to 1
 *                             it starts with the first pyramid level and so
 *                             on).
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
                                   float  c_capacity = 10.f,
                                   float  c_erosion = 0.05f,
                                   float  c_deposition = 0.01f,
                                   float  c_inertia = 0.3f,
                                   float  drag_rate = 0.01f,
                                   float  evap_rate = 0.001f,
                                   int    pyramid_finest_level = 0);

/**
 * @brief Generates a procedurally eroded terrain using hydraulic erosion and
 * ridge generation techniques.
 *
 * This function applies a combination of hydraulic erosion and ridge formation
 * to modify a heightmap, leveraging parameters such as erosion profiles, ridge
 * scaling, and noise characteristics. It also supports custom or default masks
 * to influence the erosion process.
 *
 * @param[out] z                  The heightmap to be modified, represented as a
 *                                2D array.
 * @param[in]  seed               Random seed for procedural generation.
 * @param[in]  ridge_wavelength   Wavelength of the ridge structures in the
 *                                heightmap.
 * @param[in]  ridge_scaling      Scaling factor for the ridge height.
 * @param[in]  erosion_profile    The profile that defines the erosion curve
 *                                behavior.
 * @param[in]  delta              Parameter controlling the erosion intensity.
 * @param[in]  noise_ratio        Ratio of noise added to the ridge crest lines.
 * @param[in]  prefilter_ir       Kernel radius for pre-smoothing the heightmap.
 *                                If negative, a default value is computed.
 * @param[in]  density_factor     Factor influencing the density of the ridges.
 * @param[in]  kernel_width_ratio Ratio defining the width of the ridge
 *                                generation kernel.
 * @param[in]  phase_smoothing    Smoothing factor for the phase field used in
 *                                ridge generation.
 * @param[in]  use_default_mask   Whether to use a default mask for erosion if
 *                                no mask is provided.
 * @param[in]  talus_mask         Threshold for default mask slope to identify
 *                                regions prone to erosion.
 * @param[in]  p_mask             Optional pointer to a custom mask array to
 *                                influence the erosion process.
 * @param[out] p_ridge_mask       Optional pointer to store the ridge mask
 *                                resulting from the operation.
 * @param[in]  vmin               Minimum elevation value. If set to a sentinel
 *                                value (vmax <
 * vmin), it is calculated from the heightmap.
 * @param[in]  vmax               Maximum elevation value. If set to a sentinel
 *                                value (vmax <
 * vmin), it is calculated from the heightmap.
 *
 * **Example**
 * @include ex_hydraulic_procedural.cpp
 *
 * **Result**
 * @image html ex_hydraulic_procedural0.png
 * @image html ex_hydraulic_procedural1.png
 */
void hydraulic_procedural(
    Array         &z,
    uint           seed,
    float          ridge_wavelength,
    float          ridge_scaling = 0.1f,
    ErosionProfile erosion_profile = ErosionProfile::TRIANGLE_SMOOTH,
    float          delta = 0.02f,
    float          noise_ratio = 0.2f,
    int            prefilter_ir = -1,
    float          density_factor = 1.f,
    float          kernel_width_ratio = 2.f,
    float          phase_smoothing = 2.f,
    float          phase_noise_amp = M_PI,
    bool           reverse_phase = false,
    bool           rotate90 = false,
    bool           use_default_mask = true,
    float          talus_mask = 0.f,
    Array         *p_mask = nullptr,
    Array         *p_ridge_mask = nullptr,
    float          vmin = 0.f,
    float          vmax = -1.f);

/**
 * @brief Apply hydraulic erosion based on a flow accumulation map.
 *
 * @param z                  Input array.
 * @param p_mask             Intensity mask, expected in [0, 1] (applied as a
 *                           post-processing).
 * @param c_erosion          Erosion coefficient.
 * @param talus_ref          Reference talus used to localy define the
 *                           flow-partition exponent (small values of
 *                           `talus_ref` will lead to thinner flow streams, see
 * {@link flow_accumulation_dinf}).
 * @param p_bedrock          Lower elevation limit.
 * @param p_moisture_map     Reference to the moisture map (quantity of rain),
 *                           expected to be in [0, 1].
 * @param p_erosion_map[out] Reference to the erosion map, provided as an output
 *                           field.
 * @param ir                 Kernel radius. If `ir > 1`, a cone kernel is used
 *                           to carv channel flow erosion.
 * @param clipping_ratio     Flow accumulation clipping ratio.
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
 * @brief Applies hydraulic erosion with upscaling amplification.
 *
 * This function progressively upscales the input array `z` by powers of 2 and
 * applies hydraulic erosion based on flow accumulation at each level of
 * upscaling. After all upscaling levels are processed, the array is resampled
 * back to its original resolution using bilinear interpolation.
 *
 * @param z                Input array representing elevation data.
 * @param c_erosion        Erosion coefficient.
 * @param talus_ref        Reference talus used to locally define the
 *                         flow-partition exponent. Smaller values lead to
 *                         thinner flow streams.
 * @param upscaling_levels Number of upscaling levels to apply. The function
 *                         will resample the array at each level.
 * @param persistence      A scaling factor applied at each level to adjust the
 *                         impact of the unary operation. Higher persistence
 *                         values will amplify the effects at each level.
 * @param ir               Kernel radius. If `ir > 1`, a cone kernel is used to
 *                         carve channel flow erosion.
 * @param clipping_ratio   Flow accumulation clipping ratio.
 *
 * @note The function first applies upscaling using bicubic resampling, performs
 * hydraulic erosion at each level, and finally resamples the array back to its
 * initial resolution using bilinear interpolation.
 *
 * **Example**
 * @include ex_hydraulic_stream_upscale_amplification.cpp
 *
 * **Result**
 * @image html ex_hydraulic_stream_upscale_amplification.png
 */
void hydraulic_stream_upscale_amplification(Array &z,
                                            float  c_erosion,
                                            float  talus_ref,
                                            int    upscaling_levels = 1,
                                            float  persistence = 1.f,
                                            int    ir = 1,
                                            float  clipping_ratio = 10.f);

/**
 * @brief Applies hydraulic erosion with upscaling amplification, with a
 * post-processing intensity mask.
 *
 * Similar to the overloaded version, this function progressively upscales the
 * input array `z` and applies hydraulic erosion. Additionally, an intensity
 * mask `p_mask` is applied as a post-processing step.
 *
 * @param z                Input array representing elevation data.
 * @param p_mask           Intensity mask, expected in [0, 1], which is applied
 *                         as a post-processing step.
 * @param c_erosion        Erosion coefficient.
 * @param talus_ref        Reference talus used to locally define the
 *                         flow-partition exponent. Smaller values lead to
 *                         thinner flow streams.
 * @param upscaling_levels Number of upscaling levels to apply. The function
 *                         will resample the array at each level.
 * @param persistence      A scaling factor applied at each level to adjust the
 *                         impact of the unary operation. Higher persistence
 *                         values will amplify the effects at each level.
 * @param ir               Kernel radius. If `ir > 1`, a cone kernel is used to
 *                         carve channel flow erosion.
 * @param clipping_ratio   Flow accumulation clipping ratio.
 *
 * @note This version of the function applies an additional intensity mask as
 * part of the upscaling amplification process.
 *
 * **Example**
 * @include ex_hydraulic_stream_upscale_amplification.cpp
 *
 * **Result**
 * @image html ex_hydraulic_stream_upscale_amplification.png
 */
void hydraulic_stream_upscale_amplification(
    Array &z,
    Array *p_mask,
    float  c_erosion,
    float  talus_ref,
    int    upscaling_levels = 1,
    float  persistence = 1.f,
    int    ir = 1,
    float  clipping_ratio = 10.f); ///< @overload

/**
 * @brief Apply hydraulic erosion based on a flow accumulation map, alternative
 * formulation.
 *
 * @param z                      Input array representing the terrain elevation.
 * @param c_erosion              Erosion coefficient controlling the intensity
 *                               of erosion.
 * @param talus_ref              Reference talus used to locally define the
 *                               flow-partition exponent. Small values lead to
 *                               thinner flow streams (see
 * {@link flow_accumulation_dinf}).
 * @param deposition_ir          Kernel radius for sediment deposition. If
 *                               greater than 1, a smoothing effect is applied.
 * @param deposition_scale_ratio Scaling factor for sediment deposition.
 * @param gradient_power         Exponent applied to the terrain gradient to
 *                               control erosion intensity.
 * @param gradient_scaling_ratio Scaling factor for gradient-based erosion.
 * @param gradient_prefilter_ir  Kernel radius for pre-filtering the terrain
 *                               gradient.
 * @param saturation_ratio       Ratio controlling the water saturation
 *                               threshold for erosion processes.
 * @param p_bedrock              Pointer to an optional lower elevation limit.
 * @param p_moisture_map         Pointer to the moisture map (rainfall
 *                               quantity), expected to be in [0, 1].
 * @param p_erosion_map[out]     Pointer to the erosion map, provided as an
 *                               output field.
 * @param p_flow_map[out]        Pointer to the flow accumulation map, provided
 *                               as an output field.
 * @param ir                     Kernel radius. If `ir > 1`, a cone kernel is
 *                               used to carve channel flow erosion.
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
                          int    deposition_ir = 32,
                          float  deposition_scale_ratio = 1.f,
                          float  gradient_power = 0.8f,
                          float  gradient_scaling_ratio = 1.f,
                          int    gradient_prefilter_ir = 16,
                          float  saturation_ratio = 1.f,
                          Array *p_bedrock = nullptr,
                          Array *p_moisture_map = nullptr,
                          Array *p_erosion_map = nullptr,
                          Array *p_deposition_map = nullptr,
                          Array *p_flow_map = nullptr);

void hydraulic_stream_log(Array &z,
                          float  c_erosion,
                          float  talus_ref,
                          Array *p_mask,
                          int    deposition_ir = 32,
                          float  deposition_scale_ratio = 1.f,
                          float  gradient_power = 0.8f,
                          float  gradient_scaling_ratio = 1.f,
                          int    gradient_prefilter_ir = 16,
                          float  saturation_ratio = 1.f,
                          Array *p_bedrock = nullptr,
                          Array *p_moisture_map = nullptr,
                          Array *p_erosion_map = nullptr,
                          Array *p_deposition_map = nullptr,
                          Array *p_flow_map = nullptr); ///< @overload

/**
 * @brief Apply hydraulic erosion using the 'virtual pipes' algorithm.
 *
 * See @cite Chiba1998, @cite Isheden2022, @cite Mei2007 and @cite Stava2008.
 *
 * @param z                  Input array.
 * @param p_mask             Intensity mask, expected in [0, 1] (applied as a
 * @param iterations         Number of iterations.
 * @param p_bedrock          Lower elevation limit.
 * @param p_moisture_map     Reference to the moisture map (quantity of rain),
 *                           expected to be in [0, 1].
 * @param p_erosion_map[out] Reference to the erosion map, provided as an output
 *                           field.
 * @param p_deposition_map   [out] Reference to the deposition map, provided as
 *                           an output field.
 * @param water_height       Water height.
 * @param c_capacity         Sediment capacity.
 * @param c_deposition       Deposition coefficient.
 * @param c_erosion          Erosion coefficient.
 * @param rain_rate          Rain rate.
 * @param evap_rate          Particle evaporation rate.
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
 * @param z                             Input array.
 * @param p_mask                        Intensity mask, expected in [0, 1]
 *                                      (applied as a post-processing).
 * @param talus                         Talus limit.
 * @param p_deposition_map              [out] Reference to the deposition map,
 *                                      provided as an output field.
 * @param max_deposition                Maximum height of sediment deposition.
 * @param iterations                    Number of iterations.
 * @param thermal_erosion_subiterations Number of thermal erosion iterations for
 *                                      each pass.
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
 * @param z                         Input array.
 * @param p_mask                    Intensity mask, expected in [0, 1] (applied
 *                                  as a post-processing).
 * @param nparticles                Number of particles.
 * @param ir                        Particle deposition radius.
 * @param seed                      Random seed number.
 * @param p_spawning_map            Reference to the particle spawning density
 *                                  map.
 * @param p_deposition_map          Reference to the deposition map (output).
 * @param particle_initial_sediment Initial sediment amount carried out by the
 *                                  particles.
 * @param deposition_velocity_limit Deposition at which the deposition occurs.
 * @param drag_rate                 Particle drag rate.
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

void sediment_layer(Array       &z,
                    const Array &talus_layer,
                    const Array &talus_upper_limit,
                    int          iterations,
                    bool         apply_post_filter = true,
                    Array       *p_deposition_map = nullptr);

/**
 * @brief Stratify the heightmap by creating a series of layers with elevations
 * corrected by a gamma factor.
 *
 * @param z      Input array.
 * @param p_mask Intensity mask, expected in [0, 1] (applied as a
 *               post-processing).
 * @param hs     Layer elevations. For 'n' layers, 'n + 1' values must be
 *               provided.
 * @param gamma  Layer gamma correction factors, 'n' values.
 *
 * @see          gamma_correction.
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

void stratify(Array &z,
              Array &partition,
              int    nstrata,
              float  strata_noise,
              float  gamma,
              float  gamma_noise,
              int    npartitions,
              uint   seed,
              float  mixing_gain_factor = 1.f,
              Array *p_noise = nullptr,
              float  vmin = 1.f,
              float  vmax = 0.f); ///< @overload

/**
 * @brief Stratify the heightmap by creating a multiscale series of layers with
 * elevations corrected by a gamma factor.
 *
 * @param z            Input array.
 * @param zmin         Minimum elevation for the strata
 * @param zmax         Maximum elevation for the strata
 * @param n_strata     Number of strata for each stratification iteration.
 * @param strata_noise Elevation relative noise.
 * @param gamma_list   Gamma value for each stratification iteration.
 * @param gamma_noise  Gamma relative noise.
 * @param seed         Random seed number.
 * @param p_mask       Intensity mask, expected in [0, 1] (applied as a
 *                     post-processing).
 * @param p_noise      Local elevation noise.
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
 * @param z       Input array.
 * @param p_mask  Intensity mask, expected in [0, 1] (applied as a
 *                post-processing).
 * @param hs      Layer elevations. For 'n' layers, 'n + 1' values must be
 *                provided.
 * @param gamma   Layer gamma correction factors, 'n' values.
 * @param talus   Layer talus value (slope).
 * @param angle   Slope orientation (in degrees).
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
 * Based on https://www.shadertoy.com/view/XtKSWh
 *
 * @param z                Input array.
 * @param p_mask           Filter mask, expected in [0, 1].
 * @param talus            Talus limit.
 * @param p_bedrock        Lower elevation limit.
 * @param p_deposition_map [out] Reference to the deposition map, provided as an
 *                         output field.
 * @param iterations       Number of iterations.
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
 * @see                    {@link thermal}
 *
 * @param z                Input array.
 * @param talus            Local talus limit.
 * @param iterations       Number of iterations.
 * @param p_deposition_map [out] Reference to the deposition map, provided as an
 *                         output field.
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
 * @param z          Input array.
 * @param talus      Local talus limit.
 * @param iterations Number of iterations.
 *
 * **Example**
 * @include ex_thermal_flatten.cpp
 *
 * **Result**
 * @image html ex_thermal_flatten.png
 */
void thermal_flatten(Array       &z,
                     const Array &talus,
                     const Array &bedrock,
                     int          iterations = 10,
                     int          post_filter_ir = 1);

void thermal_flatten(Array &z,
                     float  talus,
                     int    iterations = 10,
                     int    post_filter_ir = 1); ///< @overload

/**
 * @brief Apply thermal weathering erosion.
 *
 * Based on averaging over first neighbors, see \cite Olsen2004. Refer to \cite
 * Musgrave1989 for the original formulation.
 *
 * Thermal erosion refers to the process in which surface sediment weakens due
 * to temperature and detaches, falling down the slopes of the terrain until a
 * resting place is reached, where smooth plateaus tend to form
 * @cite Musgrave1989.
 *
 * @param z                Input array.
 * @param p_mask           Filter mask, expected in [0, 1].
 * @param talus            Talus limit.
 * @param p_bedrock        Lower elevation limit.
 * @param p_deposition_map [out] Reference to the deposition map, provided as an
 *                         output field.
 * @param iterations       Number of iterations.
 */
void thermal_olsen(Array       &z,
                   const Array &talus,
                   int          iterations = 10,
                   Array       *p_bedrock = nullptr,
                   Array       *p_deposition_map = nullptr);

/**
 * @brief Apply thermal erosion using a 'rib' algorithm (taken from Geomorph).
 * @param z          Input heightmap.
 * @param iterations Number of iterations.
 * @param p_bedrock  Lower elevation limit.
 *
 * **Example**
 * @include ex_thermal_rib.cpp
 *
 * **Result**
 * @image html ex_thermal_rib.png
 */
void thermal_rib(Array &z, int iterations, Array *p_bedrock = nullptr);

/**
 * @brief Applies the thermal erosion process to an array of elevation values.
 *
 * This function simulates thermal erosion by modifying the elevation values in
 * the array `z`. It compares the slope between each cell and its neighbors with
 * a specified threshold (`talus`). If the slope exceeds the threshold, material
 * is considered to move from higher to lower cells, resulting in a smoother
 * terrain.
 *
 * @param z          Reference to the array of elevation values that will be
 *                   modified.
 * @param talus      Array of threshold slope values for each cell, representing
 *                   stability criteria.
 * @param iterations Number of erosion iterations to apply.
 * @param intensity  Intensity factor controlling the amount of change per
 *                   iteration.
 *
 * **Example**
 * @include ex_thermal_schott.cpp
 *
 * **Result**
 * @image html ex_thermal_schott.png
 */
void thermal_schott(Array       &z,
                    const Array &talus,
                    int          iterations = 10,
                    float        intensity = 0.001f);

void thermal_schott(Array       &z,
                    const Array &talus,
                    Array       *p_mask,
                    int          iterations = 10,
                    float        intensity = 0.001f); ///< @overload

/**
 * @brief Applies the thermal erosion process with a uniform slope threshold.
 *
 * This overload of `thermal_schott` applies thermal erosion with a uniform
 * threshold value. It generates an internal talus map using the specified
 * constant `talus` value and applies the erosion process in the same manner as
 * the other overload.
 *
 * @param z          Reference to the array of elevation values that will be
 *                   modified.
 * @param talus      Constant threshold slope value used for all cells.
 * @param iterations Number of erosion iterations to apply.
 * @param intensity  Intensity factor controlling the amount of change per
 *                   iteration.
 *
 *  **Example**
 * @include ex_thermal_schott.cpp
 *
 * **Result**
 * @image html ex_thermal_schott.png
 */
void thermal_schott(Array      &z,
                    const float talus,
                    int         iterations = 10,
                    float       intensity = 0.001f);

void thermal_schott(Array      &z,
                    const float talus,
                    Array      *p_mask,
                    int         iterations = 10,
                    float       intensity = 0.001f); ///< @overload

} // namespace hmap

namespace hmap::gpu
{

/*! @brief See hmap::hydraulic_particle */
void hydraulic_particle(Array &z,
                        int    nparticles,
                        int    seed,
                        Array *p_bedrock = nullptr,
                        Array *p_moisture_map = nullptr,
                        Array *p_erosion_map = nullptr,
                        Array *p_deposition_map = nullptr,
                        float  c_capacity = 10.f,
                        float  c_erosion = 0.05f,
                        float  c_deposition = 0.05f,
                        float  c_inertia = 0.3f,
                        float  drag_rate = 0.001f,
                        float  evap_rate = 0.001f,
                        bool   post_filtering = false);

/*! @brief See hmap::hydraulic_particle */
void hydraulic_particle(Array &z,
                        Array *p_mask,
                        int    nparticles,
                        int    seed,
                        Array *p_bedrock = nullptr,
                        Array *p_moisture_map = nullptr,
                        Array *p_erosion_map = nullptr,
                        Array *p_deposition_map = nullptr,
                        float  c_capacity = 10.f,
                        float  c_erosion = 0.05f,
                        float  c_deposition = 0.05f,
                        float  c_inertia = 0.3f,
                        float  drag_rate = 0.001f,
                        float  evap_rate = 0.001f,
                        bool   post_filtering = false);

/**
 * @brief Simulates hydraulic erosion and deposition on a heightmap using the
 * Schott method.
 *
 * This function performs hydraulic erosion on the given heightmap `z` over a
 * specified number of iterations. It includes parameters for controlling
 * erosion, deposition, and flow routing. Optional flow accumulation can also be
 * computed and stored in the `p_flow` array.
 *
 * @param[in,out] z                      The heightmap array to be modified.
 *                                       Heights are updated in-place.
 * @param[in]     iterations             The number of iterations for the
 *                                       hydraulic erosion process.
 * @param[in]     talus                  An array defining the slope threshold
 *                                       for erosion.
 * @param[in]     c_erosion              Erosion coefficient (default: 1.0f).
 * @param[in]     c_thermal              Thermal erosion coefficient (default:
 *                                       0.1f).
 * @param[in]     c_deposition           Deposition coefficient (default: 0.2f).
 * @param[in]     flow_acc_exponent      Exponent controlling the influence of
 *                                       flow accumulation on erosion (default:
 *                                       0.8f).
 * @param[in]     flow_acc_exponent_depo Exponent controlling the influence of
 *                                       flow accumulation on deposition
 *                                       (default: 0.8f).
 * @param[in]     flow_routing_exponent  Exponent controlling flow routing
 *                                       behavior (default: 1.3f).
 * @param[in]     thermal_weight         Weight of thermal erosion effects
 *                                       (default: 1.5f).
 * @param[in]     deposition_weight      Weight of deposition effects (default:
 *                                       2.5f).
 * @param[out]    p_flow                 Optional pointer to an array for
 *                                       storing flow accumulation data. If
 *                                       null, flow data is not returned
 *                                       (default: nullptr).
 *
 * @note Taken from https://hal.science/hal-04565030v1/document
 *
 * @note Only available if OpenCL is enabled.
 *
 * **Example**
 * @include ex_hydraulic_schott.cpp
 *
 * **Result**
 * @image html ex_hydraulic_schott.png
 */
void hydraulic_schott(Array       &z,
                      int          iterations,
                      const Array &talus,
                      float        c_erosion = 1.f,
                      float        c_thermal = 0.1f,
                      float        c_deposition = 0.2f,
                      float        flow_acc_exponent = 0.8f,
                      float        flow_acc_exponent_depo = 0.8f,
                      float        flow_routing_exponent = 1.3f,
                      float        thermal_weight = 1.5f,
                      float        deposition_weight = 2.5f,
                      Array       *p_flow = nullptr);

void hydraulic_schott(Array       &z,
                      int          iterations,
                      const Array &talus,
                      Array       *p_mask,
                      float        c_erosion = 1.f,
                      float        c_thermal = 0.1f,
                      float        c_deposition = 0.2f,
                      float        flow_acc_exponent = 0.8f,
                      float        flow_acc_exponent_depo = 0.8f,
                      float        flow_routing_exponent = 1.3f,
                      float        thermal_weight = 1.5f,
                      float        deposition_weight = 2.5f,
                      Array       *p_flow = nullptr); ///< @overload

/*! @brief See hmap::hydraulic_stream_log */
void hydraulic_stream_log(Array &z,
                          float  c_erosion,
                          float  talus_ref,
                          int    deposition_ir = 32,
                          float  deposition_scale_ratio = 1.f,
                          float  gradient_power = 0.8f,
                          float  gradient_scaling_ratio = 1.f,
                          int    gradient_prefilter_ir = 16,
                          float  saturation_ratio = 1.f,
                          Array *p_bedrock = nullptr,
                          Array *p_moisture_map = nullptr,
                          Array *p_erosion_map = nullptr,
                          Array *p_deposition_map = nullptr,
                          Array *p_flow_map = nullptr);

void hydraulic_stream_log(Array &z,
                          float  c_erosion,
                          float  talus_ref,
                          Array *p_mask,
                          int    deposition_ir = 32,
                          float  deposition_scale_ratio = 1.f,
                          float  gradient_power = 0.8f,
                          float  gradient_scaling_ratio = 1.f,
                          int    gradient_prefilter_ir = 16,
                          float  saturation_ratio = 1.f,
                          Array *p_bedrock = nullptr,
                          Array *p_moisture_map = nullptr,
                          Array *p_erosion_map = nullptr,
                          Array *p_deposition_map = nullptr,
                          Array *p_flow_map = nullptr); ///< @overload

/*! @brief See hmap::thermal */
void thermal(Array       &z,
             const Array &talus,
             int          iterations = 10,
             Array       *p_bedrock = nullptr,
             Array       *p_deposition_map = nullptr);

/*! @brief See hmap::thermal */
void thermal(Array       &z,
             Array       *p_mask,
             const Array &talus,
             int          iterations = 10,
             Array       *p_bedrock = nullptr,
             Array       *p_deposition_map = nullptr);

/*! @brief See hmap::thermal */
void thermal(Array &z,
             float  talus,
             int    iterations = 10,
             Array *p_bedrock = nullptr,
             Array *p_deposition_map = nullptr);

/*! @brief See hmap::thermal_auto_bedrock */
void thermal_auto_bedrock(Array       &z,
                          const Array &talus,
                          int          iterations = 10,
                          Array       *p_deposition_map = nullptr);

/*! @brief See hmap::thermal_auto_bedrock */
void thermal_auto_bedrock(Array       &z,
                          Array       *p_mask,
                          const Array &talus,
                          int          iterations = 10,
                          Array       *p_deposition_map = nullptr);

/*! @brief See hmap::thermal_auto_bedrock */
void thermal_auto_bedrock(Array &z,
                          float,
                          int    iterations = 10,
                          Array *p_deposition_map = nullptr);

/**
 * @brief Apply thermal weathering erosion to give a scree like effect.
 *
 * @note Only available if OpenCL is enabled.
 *
 * @param z                Input array.
 * @param talus            Talus limit.
 * @param p_deposition_map [out] Reference to the deposition map, provided as an
 *                         output field.
 * @param iterations       Number of iterations.
 *
 * **Example**
 * @include thermal_ridge.cpp
 *
 * **Result**
 * @image html thermal_ridge.png
 */
void thermal_inflate(Array &z, const Array &talus, int iterations = 10);

void thermal_inflate(Array       &z,
                     const Array *p_mask,
                     const Array &talus,
                     int          iterations = 10); ///< @overload

/*! @brief See hmap::thermal_rib */
void thermal_rib(Array &z, int iterations, Array *p_bedrock = nullptr);

/**
 * @brief Apply thermal weathering erosion to give a ridge like effect.
 *
 * @note Based on https://www.fractal-landscapes.co.uk/maths.html
 *
 * @note Only available if OpenCL is enabled.
 *
 * @param z                Input array.
 * @param talus            Talus limit.
 * @param p_deposition_map [out] Reference to the deposition map, provided as an
 *                         output field.
 * @param iterations       Number of iterations.
 *
 * **Example**
 * @include thermal_ridge.cpp
 *
 * **Result**
 * @image html thermal_ridge.png
 */
void thermal_ridge(Array       &z,
                   const Array &talus,
                   int          iterations = 10,
                   Array       *p_deposition_map = nullptr);

void thermal_ridge(Array       &z,
                   const Array *p_mask,
                   const Array &talus,
                   int          iterations = 10,
                   Array       *p_deposition_map = nullptr); ///< @overload

/**
 * @brief Performs thermal scree erosion on a heightmap.
 *
 * This function applies a thermal erosion process that redistributes material
 * from steeper slopes to flatter areas, simulating talus formation. The process
 * iterates a given number of times to achieve a more stable terrain profile.
 *
 * @param[out] z                The heightmap to be modified in-place by the
 *                              erosion process.
 * @param[in]  talus            The threshold slope angles that determine where
 *                              material is moved.
 * @param[in]  zmax             The maximum allowed elevation for erosion
 *                              effects.
 * @param[in]  iterations       The number of erosion iterations to apply
 *                              (default:
 * 10).
 * @param[in]  talus_constraint Whether to enforce a constraint on the talus
 *                              slope (default: true).
 * @param[out] p_deposition_map Optional pointer to an array that stores the
 *                              deposited material per cell.
 */
void thermal_scree(Array       &z,
                   const Array &talus,
                   const Array &zmax,
                   int          iterations = 10,
                   bool         talus_constraint = true,
                   Array       *p_deposition_map = nullptr);

void thermal_scree(Array       &z,
                   const Array *p_mask,
                   const Array &talus,
                   const Array &zmax,
                   int          iterations = 10,
                   bool         talus_constraint = true,
                   Array       *p_deposition_map = nullptr); ///< @overload

} // namespace hmap::gpu
