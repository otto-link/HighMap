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
 */
enum cmap : int
{
  gray = 0, ///< gray
  jet = 1   ///< jet
};

/**
 * @brief Convert array element values to a color data (3 channels RGB in [0,
 * 255]).
 *
 * @param array Input array.
 * @param vmin Lower bound for scaling to array [0, 1].
 * @param vmax Upper bound for scaling to array [0, 1]
 * @param cmap Colormap (@see cmap).
 * @return std::vector<uint8_t> Vector containing colors (size : shape[0] *
 * shape[1] * 3 channels for RGB).
 */
std::vector<uint8_t> colorize(Array &array, float vmin, float vmax, int cmap);

void export_banner_png(std::string fname, std::vector<Array> arrays, int cmap);

} // namespace hmap