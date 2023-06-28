/**
 * @file io.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-05-08
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once
#define IMG_CHANNELS 3

namespace hmap
{

/**
 * @brief Colormaps.
 *
 * @note Use the inverse value of the cmap to reverse it. For instance
 * `hmap::cmap::gray` and `-hmap::cmap::gray`.
 */
enum cmap : int
{
  bone = 1,          ///< bone
  gray = 2,          ///< gray
  hot = 3,           ///< hot
  inferno = 4,       ///< inferno
  jet = 5,           ///< jet
  nipy_spectral = 6, ///< nipy_spectral
  seismic = 7,       ///< seismic
  terrain = 8,       ///< terrain
  viridis = 9        ///< viridis
};

/**
 * @brief Convert array element values to a color data (3 channels RGB in [0,
 * 255]).
 *
 * @param array Input array.
 * @param vmin Lower bound for scaling to array [0, 1].
 * @param vmax Upper bound for scaling to array [0, 1]
 * @param cmap Colormap (see {@link cmap}).
 * @param hillshading Activate hillshading.
 * @return std::vector<uint8_t> Vector containing colors (size : shape[0] *
 * shape[1] * 3 channels for RGB).
 */
std::vector<uint8_t> colorize(Array &array,
                              float  vmin,
                              float  vmax,
                              int    cmap,
                              bool   hillshading);

void export_banner_png(std::string        fname,
                       std::vector<Array> arrays,
                       int                cmap,
                       bool               hillshading = false);

} // namespace hmap