/**
 * @file hydrology.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-05-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

namespace hmap
{

/**
 * @brief Return flow accumulation in each cell using the D8 flow direction
 * model \cite Zhou2019.
 *
 * @param z Input array.
 * @return Array Flow accumulation.
 *
 * **Example**
 * @include ex_flow_accumulation_d8.cpp
 *
 * **Result**
 * @image html ex_flow_accumulation_d80.png
 * @image html ex_flow_accumulation_d81.png
 *
 * @see flow_direction_d8
 */
Array flow_accumulation_d8(Array &z);

/**
 * @brief Return the direction of flow from each cell to its downslope neighbor
 * using the D8 model @cite Greenlee1987.
 *
 * **Flow direction nomenclature**
 * @verbatim
   5 6 7
   4 . 0
   3 2 1
 @endverbatim
 *
 * @param z Input array.
 * @return Array Flow direction (D8 nomenclature).
 *
 * @see flow_accumulation_d8
 */
Array flow_direction_d8(Array &z);

} // namespace hmap