/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file math.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#ifdef ENABLE_OPENCL

#include "cl_wrapper.hpp"

#include "highmap/array.hpp"
#include "highmap/functions.hpp" // NoiseType

namespace hmap::gpu
{

void helper_bind_optional_buffer(clwrapper::Run    &run,
                                 const std::string &id,
                                 Array             *p_array);

bool init_opencl();

// --- function wrappers

Array closing(const Array &array, int ir);

Array dilation(const Array &array, int ir);

Array erosion(const Array &array, int ir);

void expand(Array &array, int ir);
void expand(Array &array, int ir, Array *p_mask);
void expand(Array &array, Array &kernel);
void expand(Array &array, Array &kernel, Array *p_mask);

Array gradient_norm(const Array &array);

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
                        float  drag_rate = 0.001f,
                        float  evap_rate = 0.001f,
                        bool   post_filtering = false);
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
                        float  drag_rate = 0.001f,
                        float  evap_rate = 0.001f,
                        bool   post_filtering = false);

void laplace(Array &array, float sigma = 0.2f, int iterations = 3);
void laplace(Array &array,
             Array *p_mask,
             float  sigma = 0.2f,
             int    iterations = 3);

Array maximum_local(const Array &array, int ir);

Array maximum_local_disk(const Array &array, int ir);

Array maximum_smooth(const Array &array1, const Array &array2, float k = 0.2f);

Array mean_local(const Array &array, int ir);

void median_3x3(Array &array);

Array minimum_local(const Array &array, int ir);

Array minimum_local_disk(const Array &array, int ir);

Array minimum_smooth(const Array &array1, const Array &array2, float k = 0.2f);

Array morphological_gradient(const Array &array, int ir);

Array noise(NoiseType   noise_type,
            Vec2<int>   shape,
            Vec2<float> kw,
            uint        seed,
            Array      *p_noise_x = nullptr,
            Array      *p_noise_y = nullptr,
            Array      *p_stretching = nullptr,
            Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

Array noise_fbm(NoiseType   noise_type,
                Vec2<int>   shape,
                Vec2<float> kw,
                uint        seed,
                int         octaves = 8,
                float       weight = 0.7f,
                float       persistence = 0.5f,
                float       lacunarity = 2.f,
                Array      *p_ctrl_param = nullptr,
                Array      *p_noise_x = nullptr,
                Array      *p_noise_y = nullptr,
                Array      *p_stretching = nullptr,
                Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

void normal_displacement(Array &array,
                         float  amount = 0.1f,
                         int    ir = 0,
                         bool   reverse = false);
void normal_displacement(Array &array,
                         Array *p_mask,
                         float  amount = 0.1f,
                         int    ir = 0,
                         bool   reverse = false);

Array opening(const Array &array, int ir);

void plateau(Array &array, Array *p_mask, int ir, float factor);
void plateau(Array &array, int ir, float factor);

Array relative_distance_from_skeleton(const Array &array,
                                      int          ir_search,
                                      bool         zero_at_borders = true,
                                      int          ir_erosion = 1);

Array relative_elevation(const Array &array, int ir);

Array ruggedness(const Array &array, int ir);

Array rugosity(const Array &z, int ir, bool convex = true);

void shrink(Array &array, int ir);
void shrink(Array &array, int ir, Array *p_mask);
void shrink(Array &array, Array &kernel);
void shrink(Array &array, Array &kernel, Array *p_mask);

Array skeleton(const Array &array, bool zero_at_borders = true);

void smooth_cpulse(Array &array, int ir);
void smooth_cpulse(Array &array, int ir, Array *p_mask);

void smooth_fill(Array &array,
                 int    ir,
                 float  k = 0.1f,
                 Array *p_deposition_map = nullptr);
void smooth_fill(Array &array,
                 int    ir,
                 Array *p_mask,
                 float  k = 0.1f,
                 Array *p_deposition_map = nullptr);

void thermal(Array       &z,
             const Array &talus,
             int          iterations = 10,
             Array       *p_bedrock = nullptr,
             Array       *p_deposition_map = nullptr);
void thermal(Array       &z,
             Array       *p_mask,
             const Array &talus,
             int          iterations = 10,
             Array       *p_bedrock = nullptr,
             Array       *p_deposition_map = nullptr);
void thermal(Array &z,
             float  talus,
             int    iterations = 10,
             Array *p_bedrock = nullptr,
             Array *p_deposition_map = nullptr);

void thermal_auto_bedrock(Array       &z,
                          const Array &talus,
                          int          iterations = 10,
                          Array       *p_deposition_map = nullptr);
void thermal_auto_bedrock(Array &z,
                          float,
                          int    iterations = 10,
                          Array *p_deposition_map = nullptr);

void thermal_rib(Array &z, int iterations, Array *p_bedrock = nullptr);

void warp(Array &array, Array *p_dx, Array *p_dy);

} // namespace hmap::gpu

#endif
