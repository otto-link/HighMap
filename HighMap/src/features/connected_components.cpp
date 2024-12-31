/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <map>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/boundary.hpp"

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

  for (int j = 0; j < npj; j++)
    for (int i = 0; i < npi; i++)
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
            if (lmin != v) labels_mapping[lmin].push_back(v);
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

  for (int j = 0; j < npj; j++)
    for (int i = 0; i < npi; i++)
    {
      if ((labels(i, j) > 0) & (labels_mapping_reverse.contains(labels(i, j))))
        labels(i, j) = labels_mapping_reverse[labels(i, j)];
      labels_surface[labels(i, j)] += 1.f;
    }

  // removing padding before returning the result
  labels = labels.extract_slice({1, npi - 1, 1, npj - 1});

  // --- remove component with a "small" surface
  if (surface_threshold > 0.f)
    for (int j = 0; j < labels.shape.y; j++)
      for (int i = 0; i < labels.shape.x; i++)
        if (labels_surface[labels(i, j)] < surface_threshold)
          labels(i, j) = background_value;

  return labels;
}

} // namespace hmap
