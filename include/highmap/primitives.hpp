/**
 * @file primitives.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

namespace hmap
{

/**
 * @brief Return a cone.
 *
 * Maximum value is 1.
 *
 * @param shape Array shape.
 * @return Array New array.
 */
Array cone(std::vector<int> shape);

/**
 * @brief Return a cone with a given height and talus (output array shape is
 * adjusted accordingly).
 *
 * @param height Cone height.
 * @param talus Cone talus.
 * @return Array
 */
Array cone_talus(float height, float talus);

/**
 * @brief Return an array based on a plane equation.
 *
 * @todo TO BE DONE.
 *
 * @param shape Array shape.
 * @param talus Plane talus.
 * @param yaw_angle Plane yaw_angle.
 * @param xyz_center Reference point: the plane passes through this point,
 * which can be anywhere.
 * @return Array New array.
 *
 * **Example**
 * @include ex_plane.cpp
 *
 * **Result**
 * @image html ex_plane.png
 */
Array plane(std::vector<int>   shape,
            float              talus,
            float              yaw_angle,
            std::vector<float> xyz_center);

/**
 * @brief Return a constant value array.
 *
 * @param shape Array shape.
 * @param value Filling value.
 * @return Array New array.
 */
Array constant(std::vector<int> shape, float value = 0.f);

} // namespace hmap