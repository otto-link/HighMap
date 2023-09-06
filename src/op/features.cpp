/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

#include <algorithm>
#include <map>

#include "dkm.hpp"
#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/op.hpp"
#include "highmap/primitives.hpp"

namespace hmap
{

Array connected_components(const Array &array,
                           float        surface_threshold,
                           float        background_value)
{
  // neighbor search pattern
  const std::vector<int> di = {0, -1, -1, -1};
  const std::vector<int> dj = {-1, -1, 0, 1};
  const size_t           nb = di.size();

  // padding: one cell with a non-background value on the borders
  const int npi = array.shape.x + 2;
  const int npj = array.shape.y + 2;

  Array labels = Array(Vec2<int>(npi, npj));
  Array array_pad = generate_buffered_array(array, {1, 1, 1, 1});
  set_borders(array_pad, background_value + 1.f, 1);

  // --- first pass of labelling

  int                                 current_label = 0;
  std::map<float, std::vector<float>> labels_mapping = {};

  for (int i = 0; i < npi; i++)
    for (int j = 0; j < npj; j++)
      if (array_pad(i, j) != background_value)
      {
        // scan neighbors and count those that are "background"
        std::vector<int> nbrs_label = {};
        nbrs_label.reserve(nb);

        for (size_t k = 0; k < nb; k++)
        {
          int p = i + di[k];
          int q = j + dj[k];
          if ((p > 0) and (p < npi) and (q > 0) and (q < npj))
            if (array_pad(p, q) != background_value)
              nbrs_label.push_back(labels(p, q));
        }

        if (nbrs_label.size() == 0)
          labels(i, j) = current_label++;
        else if (nbrs_label.size() == 1)
          // set current label to the value of the only
          // non-background neighbor
          labels(i, j) = nbrs_label.back();
        else
        {
          int lmin = *std::min_element(nbrs_label.begin(), nbrs_label.end());
          labels(i, j) = lmin;

          // keep track of links between label values
          for (auto &v : nbrs_label)
            if (lmin != v)
              labels_mapping[lmin].push_back(v);
        }
      }

  // --- relabel components

  // reverse the labels mapping
  std::map<float, float> labels_mapping_reverse = {};

  for (auto &[key, v] : labels_mapping)
    for (auto &s : v)
      labels_mapping_reverse[s] = key;

  // find the root label by "traversing" the mapping, starting from
  // the higher labels
  for (auto it = labels_mapping_reverse.rbegin();
       it != labels_mapping_reverse.rend();
       it++)
  {
    float label_root = it->first;
    while (true)
    {
      if (labels_mapping_reverse.contains(label_root))
        label_root = labels_mapping_reverse[label_root];
      else
        break;
    }
    it->second = label_root;
  }

  std::map<float, float> labels_surface = {};

  for (int i = 0; i < npi; i++)
    for (int j = 0; j < npj; j++)
    {
      if ((labels(i, j) > 0) & (labels_mapping_reverse.contains(labels(i, j))))
        labels(i, j) = labels_mapping_reverse[labels(i, j)];
      labels_surface[labels(i, j)] += 1.f;
    }

  // removing padding before returning the result
  labels = labels.extract_slice({1, npi - 1, 1, npj - 1});

  // --- remove component with a "small" surface
  if (surface_threshold > 0.f)
    for (int i = 0; i < labels.shape.x; i++)
      for (int j = 0; j < labels.shape.y; j++)
        if (labels_surface[labels(i, j)] < surface_threshold)
          labels(i, j) = background_value;

  return labels;
}

Array curvature_gaussian(const Array &z)
{
  Array k = Array(z.shape); // output
  Array zx = gradient_x(z);
  Array zy = gradient_y(z);
  Array zxx = gradient_x(zx);
  Array zxy = gradient_y(zx);
  Array zyy = gradient_y(zy);

  k = (zxx * zyy - pow(zxy, 2.f)) / pow(1.f + pow(zx, 2.f) + pow(zy, 2.f), 2.f);
  return k;
}

Array curvature_mean(const Array &z)
{
  Array h = Array(z.shape); // output
  Array zx = gradient_x(z);
  Array zy = gradient_y(z);
  Array zxx = gradient_x(zx);
  Array zxy = gradient_y(zx);
  Array zyy = gradient_y(zy);

  h = (zxx * (1.f + zy * zy) - 2.f * zxy * zx * zy + zyy * (1.f + zx * zx)) *
      0.5f / pow(1.f + zx * zx + zy * zy, 1.5f);
  return h;
}

// HELPER
bool cmp_inf(std::pair<float, float> &a, std::pair<float, float> &b)
{
  return (a.first < b.first) | ((a.first == b.first) & (a.second < b.second));
}

Array kmeans_clustering2(const Array &array1,
                         const Array &array2,
                         int          nclusters,
                         uint         seed)
{
  Vec2<int> shape = array1.shape;
  Array     kmeans = Array(shape); // output

  // recast data
  std::vector<std::array<float, 2>> data = {};
  data.resize(shape.x * shape.y);

  for (int i = 0; i < shape.x; i++)
    for (int j = 0; j < shape.y; j++)
    {
      int k = i + j * shape.x;
      data[k][0] = array1(i, j);
      data[k][1] = array2(i, j);
    }

  dkm::clustering_parameters<float> parameters =
      dkm::clustering_parameters<float>(nclusters);
  parameters.set_random_seed(seed);
  auto dkm = dkm::kmeans_lloyd(data, parameters);

  // modify labelling to ensure it remains fairly consistent when the
  // data are modified (centroid are sorted by their coordinates)
  std::vector<int>   isort_rev(nclusters);
  std::vector<Point> centroids = {};

  for (auto &p : std::get<0>(dkm))
    centroids.push_back(Point(p[0], p[1]));

  sort_points(centroids);

  // TODO dirty
  for (int i = 0; i < nclusters; i++)
    for (int j = 0; j < nclusters; j++)
      if ((centroids[i].x == std::get<0>(dkm)[j][0]) &
          (centroids[i].y == std::get<0>(dkm)[j][1]))
        isort_rev[j] = i;

  for (size_t k = 0; k < std::get<1>(dkm).size(); k++)
  {
    int j = int(k / shape.x);
    int i = k - j * shape.x;
    kmeans(i, j) = isort_rev[std::get<1>(dkm)[k]];
  }

  return kmeans;
}

Array rugosity(const Array &z, int ir)
{
  hmap::Array z_avg = Array(z.shape);
  hmap::Array z_std = Array(z.shape);
  hmap::Array z_skw = Array(z.shape); // output

  z_avg = mean_local(z, ir);

  // standard deviation
  {
    hmap::Array z2 = pow(z, 2.f);
    z_std = pow(mean_local(z2, ir) - pow(z_avg, 2.f), 0.5f);
  }

  // skewness
  {
    hmap::Array z3 = pow(z, 3.f);
    z_skw = (mean_local(z3, ir) - pow(z_avg, 3.f)) / pow(z_std, 3.f);
  }

  return z_skw;
}

Array valley_width(const Array &z, int ir)
{
  Array vw = z;
  if (ir > 0)
    smooth_cpulse(vw, ir);

  vw = curvature_mean(-vw);
  vw = distance_transform(vw);
  return vw;
}

} // namespace hmap
