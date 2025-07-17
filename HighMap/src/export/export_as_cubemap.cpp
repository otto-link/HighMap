/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/export.hpp"
#include "highmap/filters.hpp"
#include "highmap/kernels.hpp"
#include "highmap/operator.hpp"
#include "highmap/range.hpp"
#include "highmap/transform.hpp"

namespace hmap
{

//----------------------------------------------------------------------
// Helpers
//----------------------------------------------------------------------

void helper_smooth_corners(Array    &cubemap,
                           int       noverlap,
                           int       ir,
                           Vec4<int> idx_front,
                           Vec4<int> idx_back)
{
  Array mask(cubemap.shape);

  // horizontal
  for (int r = -noverlap + 1; r < noverlap; r++)
  {
    mask(idx_front.a + r, idx_front.c) = 1.f;
    mask(idx_front.a + r, idx_front.d) = 1.f;
    mask(idx_front.b + r, idx_front.c) = 1.f;
    mask(idx_front.b + r, idx_front.d) = 1.f;
  }

  smooth_cpulse(mask, 2 * ir);
  remap(mask);
  smooth_cpulse(cubemap, ir, &mask);

  // vertical
  mask = 0.f;
  for (int r = -noverlap + 1; r < noverlap; r++)
  {
    mask(idx_front.a, idx_front.c + r) = 1.f;
    mask(idx_front.b, idx_front.c + r) = 1.f;
    mask(idx_front.a, idx_front.d + r) = 1.f;
    mask(idx_front.b, idx_front.d + r) = 1.f;

    mask(idx_back.a, idx_back.c + r) = 1.f;
    mask(idx_back.a, idx_back.d + r) = 1.f;
  }

  smooth_cpulse(mask, 2 * ir);
  remap(mask);
  smooth_cpulse(cubemap, ir, &mask);
}

void helper_get_rtheta_stretch(int    i,
                               int    j,
                               int    ic,
                               int    jc,
                               int    nradius,
                               float &radius,
                               float &theta,
                               int    config)
{
  radius = std::hypot(i - ic, j - jc) / static_cast<float>(nradius);
  theta = std::atan2(j - jc, i - ic);

  if (config == 0)
    theta += 0.5f * M_PI;
  else if (config == 1)
    theta = theta < -0.5f * M_PI ? 2.f * M_PI + theta : theta;
  else if (config == 2)
    theta = theta < 0.5f * M_PI ? theta + M_PI : -1.f;
  else if (config == 3)
  {
    theta -= 0.5f * M_PI;
    theta = theta < -0.5f * M_PI ? 2.f * M_PI + theta : theta;
  }

  theta *= 4.f / 3.f;
}

float helper_get_distance_polar(float r1, float theta1, float r2, float theta2)
{
  return std::sqrt(r1 * r1 + r2 * r2 -
                   2.f * r1 * r2 * std::cos(theta1 - theta2));
}

void helper_smooth_triple_corner(Array &zfull,
                                 int    ic,
                                 int    jc,
                                 int    noverlap,
                                 int    ir,
                                 int    config)
{
  LOG_DEBUG("triple corner smoothing");

  int nradius = static_cast<int>(1.5f * noverlap);

  for (int p = -nradius; p < nradius + 1; p++)
    for (int q = -nradius; q < nradius + 1; q++)
    {
      int i = ic + p;
      int j = jc + q;

      float radius, theta;
      helper_get_rtheta_stretch(i, j, ic, jc, nradius, radius, theta, config);

      if (theta >= 0.f && radius <= 1.f)
      {
        // average
        float sum = 0.f;
        float count = 0;

        for (int r = -nradius; r < nradius + 1; r++)
          for (int s = -nradius; s < nradius + 1; s++)
          {
            float radius_avg, theta_avg;

            helper_get_rtheta_stretch(i + r,
                                      j + s,
                                      ic,
                                      jc,
                                      nradius,
                                      radius_avg,
                                      theta_avg,
                                      config);

            if (theta_avg >= 0.f)
            {

              float dist = helper_get_distance_polar(radius,
                                                     theta,
                                                     radius_avg,
                                                     theta_avg);

              float dist_relative = (float)nradius * dist / (float)ir;

              if (dist_relative < 1.f)
              {
                sum += (1.f - dist_relative) * zfull(i + r, j + s);
                count += (1.f - dist_relative);
              }
            }
          }

        float rlim = 0.6f;
        float amp = radius < rlim ? 1.f : (radius - 1.f) / (rlim - 1.f);
        amp = smoothstep3(amp);

        float cexp = 1.f;
        amp *= std::exp(-cexp * theta * theta) +
               std::exp(-cexp * (theta - 0.666f * M_PI) *
                        (theta - 0.666f * M_PI)) +
               std::exp(-cexp * (theta - 1.333f * M_PI) *
                        (theta - 1.333f * M_PI)) +
               std::exp(-cexp * (theta - 2.f * M_PI) * (theta - 2.f * M_PI));

        zfull(i, j) = amp * sum / count + (1.f - amp) * zfull(i, j);
        // zfull(i, j) = theta;
        // zfull(i, j) = amp;
      }
    }
}

//----------------------------------------------------------------------
// Main operator
//----------------------------------------------------------------------

void export_as_cubemap(const std::string &fname,
                       const Array       &z,
                       int                cubemap_resolution,
                       float              overlap,
                       int                ir,
                       Cmap               cmap,
                       bool               splitted,
                       Array             *p_cubemap)
{

  // shape of individual texture of the cubemap (work with square
  // arrays to simplify and fasten things...)
  Vec2<int> cm_shape(cubemap_resolution, cubemap_resolution);

  int noverlap = static_cast<int>(overlap * cubemap_resolution);

  LOG_DEBUG("cubemap_resolution: %d", cubemap_resolution);
  LOG_DEBUG("noverlap: %d", noverlap);
  LOG_DEBUG("overlap: %f", overlap);

  // rebuild a larger arrays with buffer arrays and the proper
  // shape. In this array must fit 4 arrays + their buffers in the
  // width and 4 array + their buffers in the height

  Vec2<int> work_shape(4 * cm_shape.x + 2 * noverlap,
                       3 * cm_shape.y + 2 * noverlap);

  Array zfull = z.resample_to_shape_bicubic(work_shape);

  // zfull.to_png("out0.png", Cmap::TERRAIN, true); // DBG

  // extract six arrays from the cubemap
  Array z_left(cm_shape);
  Array z_right(cm_shape);
  Array z_top(cm_shape);
  Array z_bottom(cm_shape);
  Array z_front(cm_shape);
  Array z_back(cm_shape);

  // define indices of the bottom-left cell of each map
  int i_left = noverlap;
  int j_left = noverlap + cm_shape.y;

  int i_front = noverlap + cm_shape.x;
  int j_front = j_left;

  int i_right = noverlap + 2 * cm_shape.x;
  int j_right = j_left;

  int i_back = noverlap + 3 * cm_shape.x;
  int j_back = j_left;

  int i_bottom = i_front;
  int j_bottom = noverlap;

  int i_top = i_front;
  int j_top = noverlap + 2 * cm_shape.y;

  Vec4<int> idx_left(i_left, i_left + cm_shape.x, j_left, j_left + cm_shape.y);

  Vec4<int> idx_right(i_right,
                      i_right + cm_shape.x,
                      j_right,
                      j_right + cm_shape.y);

  Vec4<int> idx_top(i_top, i_top + cm_shape.x, j_top, j_top + cm_shape.y);

  Vec4<int> idx_bottom(i_bottom,
                       i_bottom + cm_shape.x,
                       j_bottom,
                       j_bottom + cm_shape.y);

  Vec4<int> idx_front(i_front,
                      i_front + cm_shape.x,
                      j_front,
                      j_front + cm_shape.y);

  Vec4<int> idx_back(i_back, i_back + cm_shape.x, j_back, j_back + cm_shape.y);

  // --- enforce continuity

  LOG_DEBUG("enforce base continuity");

  // coefficient to smooth the transition
  std::vector<float> t = linspace(0.f, 0.5f, noverlap, true);

  for (auto &v : t)
    v = 0.5f * smoothstep3(2.f * v);

  Array zfull_bckp;

  zfull_bckp = zfull;

  if (true)
  {
    // left/back
    for (int k = 0; k < cubemap_resolution; k++)
      for (int r = 0; r < noverlap; r++)
      {
        zfull(idx_left.a + r,
              idx_left.c + k) = (0.5f + t[r]) *
                                    zfull_bckp(idx_left.a + r, idx_left.c + k) +
                                (0.5f - t[r]) *
                                    zfull_bckp(idx_back.b + r, idx_back.c + k);

        zfull(idx_back.a + cm_shape.x - r,
              idx_left.c + k) = (0.5f + t[r]) *
                                    zfull(idx_back.a + cm_shape.x - r,
                                          idx_left.c + k) +
                                (0.5f - t[r]) *
                                    zfull_bckp(idx_left.a - r, idx_left.c + k);
      }
    zfull_bckp = zfull;

    // bottom low/back low
    for (int k = 0; k < cubemap_resolution; k++)
      for (int r = 0; r < noverlap; r++)
      {
        zfull(idx_bottom.a + k,
              idx_bottom.c + r) = (0.5f + t[r]) * zfull_bckp(idx_bottom.a + k,
                                                             idx_bottom.c + r) +
                                  (0.5f - t[r]) * zfull_bckp(idx_back.b - k,
                                                             idx_back.c - r);

        zfull(idx_back.a + k,
              idx_back.c + r) = (0.5f + t[r]) *
                                    zfull_bckp(idx_back.a + k, idx_back.c + r) +
                                (0.5f - t[r]) * zfull_bckp(idx_bottom.b - k,
                                                           idx_bottom.c - r);
      }
    zfull_bckp = zfull;

    // top high/back high
    for (int k = 0; k < cubemap_resolution; k++)
      for (int r = 0; r < noverlap; r++)
      {
        zfull(idx_top.a + k,
              idx_top.d - r) = (0.5f + t[r]) *
                                   zfull_bckp(idx_top.a + k, idx_top.d - r) +
                               (0.5f - t[r]) *
                                   zfull_bckp(idx_back.b - k, idx_back.d + r);

        // zfull(idx_top.a + k,
        //       idx_top.d - r) =zfull_bckp(idx_back.b - k, idx_back.d + r);

        zfull(idx_back.a + k,
              idx_back.d - r) = (0.5f + t[r]) *
                                    zfull_bckp(idx_back.a + k, idx_back.d - r) +
                                (0.5f - t[r]) *
                                    zfull_bckp(idx_top.b - k, idx_top.d + r);
      }
    zfull_bckp = zfull;

    for (int k = 0; k < cubemap_resolution; k++)
      for (int r = 0; r < noverlap; r++)
      {
        // left/top
        zfull(idx_left.a + k,
              idx_left.d - r) = (0.5f + t[r]) *
                                    zfull_bckp(idx_left.a + k, idx_left.d - r) +
                                (0.5f - t[r]) *
                                    zfull_bckp(idx_top.a - r, idx_top.d - k);

        zfull(idx_top.a + r,
              idx_top.c + k) = (0.5f + t[r]) *
                                   zfull_bckp(idx_top.a + r, idx_top.c + k) +
                               (0.5f - t[r]) *
                                   zfull_bckp(idx_left.b - k, idx_left.d + r);

        // left/bottom
        zfull(idx_left.a + k,
              idx_left.c + r) = (0.5f + t[r]) *
                                    zfull_bckp(idx_left.a + k, idx_left.c + r) +
                                (0.5f - t[r]) * zfull_bckp(idx_bottom.a - r,
                                                           idx_bottom.c + k);

        zfull(idx_bottom.a + r,
              idx_bottom.c + k) = (0.5f + t[r]) * zfull_bckp(idx_bottom.a + r,
                                                             idx_bottom.c + k) +
                                  (0.5f - t[r]) * zfull_bckp(idx_left.a + k,
                                                             idx_left.c - r);

        // top/right
        zfull(idx_right.a + k,
              idx_right.d - r) = (0.5f + t[r]) * zfull_bckp(idx_right.a + k,
                                                            idx_right.d - r) +
                                 (0.5f - t[r]) *
                                     zfull_bckp(idx_top.b + r, idx_top.c + k);

        zfull(idx_top.b - r,
              idx_top.c + k) = (0.5f + t[r]) *
                                   zfull_bckp(idx_top.b - r, idx_top.c + k) +
                               (0.5f - t[r]) *
                                   zfull_bckp(idx_right.a + k, idx_right.d + r);

        // right/bottom
        zfull(idx_right.a + k,
              idx_right.c + r) = (0.5f + t[r]) * zfull_bckp(idx_right.a + k,
                                                            idx_right.c + r) +
                                 (0.5f - t[r]) * zfull_bckp(idx_bottom.b + r,
                                                            idx_bottom.d - k);

        zfull(idx_bottom.b - r,
              idx_bottom.c + k) = (0.5f + t[r]) * zfull_bckp(idx_bottom.b - r,
                                                             idx_bottom.c + k) +
                                  (0.5f - t[r]) * zfull_bckp(idx_right.b - k,
                                                             idx_right.c - r);
      }
  }

  // --- triple corners

  LOG_DEBUG("triple corners: 1 to 4");

  helper_smooth_triple_corner(zfull,
                              idx_front.a - 1,
                              idx_front.c,
                              noverlap,
                              ir,
                              0);
  helper_smooth_triple_corner(zfull, idx_front.b, idx_front.c, noverlap, ir, 1);
  helper_smooth_triple_corner(zfull,
                              idx_front.a - 1,
                              idx_front.d,
                              noverlap,
                              ir,
                              2);
  helper_smooth_triple_corner(zfull, idx_front.b, idx_front.d, noverlap, ir, 3);

  // zfull.to_png("cubemap3.png", Cmap::TERRAIN, false);

  LOG_DEBUG("triple corners: 5 to 8");

  LOG_DEBUG("reorganize array");

  // put the back in the center of the cubemap

  // extract base data
  z_left = zfull.extract_slice(idx_left);
  z_right = zfull.extract_slice(idx_right);
  z_top = zfull.extract_slice(idx_top);
  z_bottom = zfull.extract_slice(idx_bottom);
  z_front = zfull.extract_slice(idx_front);
  z_back = zfull.extract_slice(idx_back);

  zfull_bckp = 0.f;

  zfull_bckp.set_slice(idx_front, z_back);
  zfull_bckp.set_slice(idx_back, z_front);
  zfull_bckp.set_slice(idx_left, z_right);
  zfull_bckp.set_slice(idx_right, z_left);

  rot180(z_top);
  zfull_bckp.set_slice(idx_top, z_top);

  rot180(z_bottom);
  zfull_bckp.set_slice(idx_bottom, z_bottom);

  helper_smooth_triple_corner(zfull_bckp,
                              idx_front.a - 1,
                              idx_front.c,
                              noverlap,
                              ir,
                              0);
  helper_smooth_triple_corner(zfull_bckp,
                              idx_front.b,
                              idx_front.c,
                              noverlap,
                              ir,
                              1);
  helper_smooth_triple_corner(zfull_bckp,
                              idx_front.a - 1,
                              idx_front.d,
                              noverlap,
                              ir,
                              2);
  helper_smooth_triple_corner(zfull_bckp,
                              idx_front.b,
                              idx_front.d,
                              noverlap,
                              ir,
                              3);

  // send back to orignal positions
  z_left = zfull_bckp.extract_slice(idx_left);
  z_right = zfull_bckp.extract_slice(idx_right);
  z_top = zfull_bckp.extract_slice(idx_top);
  z_bottom = zfull_bckp.extract_slice(idx_bottom);
  z_front = zfull_bckp.extract_slice(idx_front);
  z_back = zfull_bckp.extract_slice(idx_back);

  zfull = 0.f;
  zfull.set_slice(idx_front, z_back);
  zfull.set_slice(idx_back, z_front);
  zfull.set_slice(idx_left, z_right);
  zfull.set_slice(idx_right, z_left);

  rot180(z_top);
  zfull.set_slice(idx_top, z_top);

  rot180(z_bottom);
  zfull.set_slice(idx_bottom, z_bottom);

  // --- extract maps

  LOG_DEBUG("extract map");

  // return cubemap array if requested
  if (p_cubemap) *p_cubemap = zfull;

  if (splitted)
  {
    z_left = zfull.extract_slice(idx_left);
    z_right = zfull.extract_slice(idx_right);
    z_top = zfull.extract_slice(idx_top);
    z_bottom = zfull.extract_slice(idx_bottom);
    z_front = zfull.extract_slice(idx_front);
    z_back = zfull.extract_slice(idx_back);

    std::filesystem::path path(fname);

    z_right.to_png(add_filename_suffix(path, "_00_right").string(), cmap);
    z_left.to_png(add_filename_suffix(path, "_01_left").string(), cmap);
    z_top.to_png(add_filename_suffix(path, "_02_top").string(), cmap);
    z_bottom.to_png(add_filename_suffix(path, "_03_bottom").string(), cmap);
    z_front.to_png(add_filename_suffix(path, "_04_front").string(), cmap);
    z_back.to_png(add_filename_suffix(path, "_05_back").string(), cmap);
  }
  else
  {
    // remove buffers
    zfull = zfull.extract_slice(Vec4<int>(noverlap,
                                          zfull.shape.x - noverlap,
                                          noverlap,
                                          zfull.shape.y - noverlap));
    zfull.to_png(fname, cmap, false);
  }
}

} // namespace hmap
