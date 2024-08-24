/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */

/**
 * @file path.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief Path class for manipulating and analyzing paths in 2D space.
 *
 * This file defines the `Path` class, which extends the `Cloud` class to
 * provide functionalities specific to paths. The `Path` class includes methods
 * for creating, manipulating, and analyzing paths, including smoothing the path
 * with various curves, resampling, and performing operations like meandering
 * and fractalizing.
 *
 * @version 0.1
 * @date 2023-06-18
 *
 * @copyright Copyright (c) 2023 Otto Link
 *
 * This software is distributed under the terms of the GNU General Public
 * License. The full license is in the file LICENSE, distributed with this
 * software.
 */
#pragma once
#include "highmap/geometry/cloud.hpp"

namespace hmap
{

/**
 * @class Path
 * @brief Represents an ordered set of points in 2D, forming a polyline (open or
 * closed).
 *
 * The `Path` class extends the `Cloud` class to represent a sequence of points
 * in 2D space that are connected in a specific order, forming a polyline. The
 * polyline can be either open or closed, depending on whether the first and
 * last points are connected. This class provides methods for various geometric
 * operations, including path length calculation, point insertion, and more.
 *
 * **Example**
 * @include ex_path.cpp
 *
 * **Result**
 * @image html ex_path.png
 */
class Path : public Cloud
{
public:
  /**
   * @brief Defines whether the path is closed or open.
   * If `true`, the path is closed, forming a loop. If `false`, the path is
   * open.
   */
  bool closed;

  /**
   * @brief Construct a new Path object with default properties.
   * Initializes an empty path with the `closed` property set to `false`.
   * @param closed Open/close path flag.
   */
  Path(bool closed = false) : Cloud(), closed(closed){};

  /**
   * @brief Construct a new Path object with random positions and values.
   * Initializes a path with a specified number of points, random values,
   * and the option to be open or closed.
   * @param npoints Number of points to generate.
   * @param seed Random seed number for generating random values.
   * @param bbox Bounding box for random point generation.
   * @param closed Open/close path flag.
   */
  Path(int         npoints,
       uint        seed,
       Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f},
       bool        closed = false)
      : Cloud(npoints, seed, bbox), closed(closed){};

  /**
   * @brief Construct a new Path object based on a list of points.
   * Initializes a path with the specified points and an option to be open or
   * closed.
   * @param points List of points defining the path.
   * @param closed Open/close path flag.
   */
  Path(std::vector<Point> points, bool closed = false)
      : Cloud(points), closed(closed){};

  /**
   * @brief Construct a new Path object based on `x` and `y` coordinates.
   * Initializes a path with the specified `x` and `y` coordinates and an option
   * to be open or closed.
   * @param x List of `x` coordinates for the points.
   * @param y List of `y` coordinates for the points.
   * @param closed Open/close path flag.
   */
  Path(std::vector<float> x, std::vector<float> y, bool closed = false)
      : Cloud(x, y), closed(closed){};

  /**
   * @brief Construct a new Path object based on `x`, `y` coordinates, and
   * values. Initializes a path with the specified `x` and `y` coordinates,
   * associated values, and an option to be open or closed.
   * @param x List of `x` coordinates for the points.
   * @param y List of `y` coordinates for the points.
   * @param v List of values associated with the points.
   * @param closed Open/close path flag.
   */
  Path(std::vector<float> x,
       std::vector<float> y,
       std::vector<float> v,
       bool               closed = false)
      : Cloud(x, y, v), closed(closed){};

  /**
   * @brief Smooth the path using Bezier curves.
   *
   * This method applies Bezier curve smoothing to the path. The
   * `curvature_ratio` controls the amount of curvature applied, with typical
   * values in the range of [-1, 1], where positive values generally result in
   * more pronounced curvature. The `edge_divisions` parameter determines the
   * number of subdivisions per edge to create a smoother curve.
   *
   * **Example**
   * @include ex_path_bezier.cpp
   *
   * **Result**
   * @image html ex_path_bezier.png
   *
   * @param curvature_ratio Amount of curvature, usually in the range [-1, 1],
   * with positive values resulting in more curvature.
   * @param edge_divisions Number of subdivisions per edge to achieve smooth
   * curves.
   */
  void bezier(float curvature_ratio = 0.3f, int edge_divisions = 10);

  /**
   * @brief Smooth the path using Bezier curves (alternative method).
   *
   * This alternative method applies Bezier curve smoothing to the path. The
   * `curvature_ratio` parameter affects the curvature amount, similar to the
   * `bezier` method. The `edge_divisions` parameter specifies how finely each
   * edge is divided to create a smoother curve.
   *
   * **Example**
   * @include ex_path_bezier_round.cpp
   *
   * **Result**
   * @image html ex_path_bezier_round.png
   *
   * @param curvature_ratio Amount of curvature, typically within [-1, 1], with
   * positive values for increased curvature.
   * @param edge_divisions Number of edge subdivisions for smoothness.
   */
  void bezier_round(float curvature_ratio = 0.3f, int edge_divisions = 10);

  /**
   * @brief Smooth the path using B-Spline curves.
   *
   * This method smooths the path using B-Spline curves, which provide a smooth
   * curve that passes through the control points. The `edge_divisions`
   * parameter defines the number of subdivisions per edge for achieving smooth
   * curves.
   *
   * **Important**: This function does not correctly handle closed polylines
   * (circular contours). If the path is closed, the smoothing may not correctly
   * close the loop, potentially leaving a gap between the start and end points.
   *
   * **Example**
   * @include ex_path_bspline.cpp
   *
   * **Result**
   * @image html ex_path_bspline.png
   *
   * @param edge_divisions Number of subdivisions per edge to achieve a smooth
   * B-Spline curve.
   *
   * @warning This function does not correctly handle closed polylines.
   */
  void bspline(int edge_divisions = 10);

  /**
   * @brief Smooth the path using Catmull-Rom curves.
   *
   * This method applies Catmull-Rom curve smoothing to the path. Catmull-Rom
   * splines are interpolating splines that pass through each control point. The
   * `edge_divisions` parameter determines the number of subdivisions per edge
   * for smoothing.
   *
   * **Important**: This function does not correctly handle closed polylines
   * (circular contours). If the path is closed, the smoothing may not correctly
   * close the loop, potentially leaving a gap between the start and end points.
   *
   * **Example**
   * @include ex_path_catmullrom.cpp
   *
   * **Result**
   * @image html ex_path_catmullrom.png
   *
   * @param edge_divisions Number of edge subdivisions to create a smooth
   * Catmull-Rom curve.
   *
   * @warning This function does not correctly handle closed polylines.
   */
  void catmullrom(int edge_divisions = 10);

  /**
   * @brief Clear the path data.
   *
   * This method removes all points and associated data from the path,
   * effectively resetting it to an empty state.
   */
  void clear();

  /**
   * @brief Smooth the path using De Casteljau curves.
   *
   * This function smooths a path by applying De Casteljau's algorithm to
   * generate intermediate points along the path, effectively creating a Bézier
   * curve that approximates the original path. The path is divided into
   * segments, and the De Casteljau algorithm is applied to each segment,
   * resulting in a smooth curve.
   *
   * The parameter `edge_divisions` controls the number of divisions
   * (sub-segments) created along each segment of the path. A higher number of
   * divisions will result in a smoother curve, but will also increase the
   * computational cost.
   *
   * @param edge_divisions The number of divisions for each edge of the path.
   *                       Default is 10, which provides a balanced level of
   * smoothing.
   *
   * **Example**
   * @include ex_path_decasteljau.cpp
   *
   * **Result**
   * @image html ex_path_decasteljau.png
   */
  void decasteljau(int edge_divisions = 10);

  /**
   * @brief Divide the path by adding points based on the lowest elevation
   * difference between each pair of edge endpoints.
   *
   * This method uses the elevation map to subdivide the path by adding
   * intermediate points where the elevation difference between edges is
   * minimal. The `elevation_ratio` parameter balances the influence of absolute
   * elevation versus elevation difference in the cost function used for path
   * finding. The `distance_exponent` affects the weight function used in
   * Dijkstra's algorithm. Areas defined by the `p_mask_nogo` mask are avoided.
   *
   * **Example**
   * @include ex_path_dijkstra.cpp
   *
   * **Result**
   * @image html ex_path_dijkstra.png
   *
   * @param array Elevation map used to determine elevation differences along
   * the path.
   * @param bbox Bounding box of the domain, defining the area where elevation
   * data is valid.
   * @param edge_divisions Number of subdivisions per edge; set to 0 for
   * automatic division based on array shape.
   * @param elevation_ratio Ratio used to balance absolute elevation and
   * elevation difference in the cost function.
   * @param distance_exponent Exponent used in the Dijkstra weight function to
   * adjust the influence of distance.
   * @param p_mask_nogo Optional mask array defining areas to avoid; points in
   * these areas will not be considered.
   *
   * @see Array::find_path_dijkstra
   */
  void dijkstra(Array      &array,
                Vec4<float> bbox,
                int         edge_divisions = 0,
                float       elevation_ratio = 0.f,
                float       distance_exponent = 0.5f,
                Array      *p_mask_nogo = nullptr);

  /**
   * @brief Divide the path by adding a point between each pair of consecutive
   * points.
   *
   * This method adds new points in the middle of each segment of the path to
   * create a denser set of points along the path. This is useful for increasing
   * the resolution of the path.
   */
  void divide();

  /**
   * @brief Applies fractalization to the path by adding points and randomly
   * displacing their positions.
   *
   * This method enhances the complexity of a path by iteratively adding new
   * points between existing ones and displacing them using Gaussian noise. The
   * process can simulate natural phenomena like terrain generation or random
   * walk paths. The number of iterations determines the level of detail added
   * to the path.
   *
   * - `sigma` controls the magnitude of the displacement, normalized by the
   * distance between points.
   * - `orientation` directs the displacement:
   *    - `0` for random directions,
   *    - `1` for inflation (outward displacement),
   *    - `-1` for deflation (inward displacement).
   * - `persistence` governs how the noise strength evolves over iterations,
   * typically reducing it gradually.
   * - An optional `control_field` array allows for local adjustments of
   * displacement amplitude, guided by the `bbox` (bounding box), which defines
   * the spatial extent within which the control field is applied.
   *
   * **Example**
   * @include ex_path_fractalize.cpp
   *
   * **Result**
   * @image html ex_path_fractalize.png
   *
   * @param iterations Number of iterations to apply the fractalization process.
   * @param seed Seed value for random number generation, ensuring
   * reproducibility.
   * @param sigma Standard deviation of the Gaussian displacement, relative to
   * the distance between points.
   * @param orientation Determines the displacement direction: `0` for random,
   * `1` for inflation, `-1` for deflation.
   * @param persistence Factor that adjusts the noise intensity across
   * iterations.
   * @param control_field Optional pointer to an array that locally modifies the
   * displacement amplitude.
   * @param bbox Bounding box that defines the valid area for the control
   * field's influence.
   */
  void fractalize(int         iterations,
                  uint        seed,
                  float       sigma = 0.3f,
                  int         orientation = 0,
                  float       persistence = 1.f,
                  Array      *p_control_field = nullptr,
                  Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f});

  /**
   * @brief Get the arc length of the path.
   *
   * The arc length is the cumulative distance along the path, normalized to the
   * range [0, 1]. This represents the distance traveled from the start to each
   * point along the path as a fraction of the total path length.
   *
   * @return std::vector<float> Vector of arc length values, where each entry
   * corresponds to a point on the path and represents the normalized distance
   * from the start of the path to that point.
   */
  std::vector<float> get_arc_length();

  /**
   * @brief Get the cumulative distance of the path.
   *
   * This method computes the cumulative distance along the path, which is the
   * total distance traveled up to each point on the path. It accumulates the
   * distances from the start of the path to each point.
   *
   * @return std::vector<float> Vector of cumulative distance values, where each
   * entry represents the distance from the start of the path to the respective
   * point.
   */
  std::vector<float> get_cumulative_distance();

  /**
   * @brief Get the values assigned to the points on the path.
   *
   * This method retrieves the values assigned to each point on the path. These
   * values can represent any attribute associated with the points, such as
   * color, intensity, or other metrics.
   *
   * @return std::vector<float> Vector of values assigned to the points on the
   * path.
   */
  std::vector<float> get_values() const;

  /**
   * @brief Get the `x` coordinates of the points on the path.
   *
   * This method retrieves the `x` coordinates of all points in the path. Each
   * value in the returned vector corresponds to the `x` coordinate of a point
   * along the path.
   *
   * @return std::vector<float> Vector of `x` values of the points on the path.
   */
  std::vector<float> get_x() const;

  /**
   * @brief Get the coordinates of the points as a single vector.
   *
   * This method returns the coordinates of the points in the path as a single
   * vector, where the coordinates are interleaved: `(x0, y0, x1, y1, ...)`.
   * This format is useful for operations that require a flat representation of
   * the coordinates.
   *
   * @return std::vector<float> Vector of interleaved `x` and `y` coordinates of
   * the points on the path.
   */
  std::vector<float> get_xy() const;

  /**
   * @brief Get the `y` coordinates of the points on the path.
   *
   * This method retrieves the `y` coordinates of all points in the path. Each
   * value in the returned vector corresponds to the `y` coordinate of a point
   * along the path.
   *
   * @return std::vector<float> Vector of `y` values of the points on the path.
   */
  std::vector<float> get_y() const;

  /**
   * @brief Add "meanders" to the path.
   *
   * This method introduces meandering effects to the path by adding random
   * deviations. The amplitude of the meanders is controlled by the `ratio`
   * parameter, while the `noise_ratio` controls the amount of randomness. The
   * `seed` parameter is used to initialize the random number generator,
   * ensuring reproducibility. The `iterations` parameter defines how many times
   * the meandering process is applied, and `edge_divisions` controls how finely
   * each edge is subdivided during the meandering.
   *
   * **Example**
   * @include ex_path_meanderize.cpp
   *
   * **Result**
   * @image html ex_path_meanderize.png
   *
   * @param ratio Amplitude ratio of the meanders. Typically a positive value.
   * @param noise_ratio Ratio of randomness introduced during meandering.
   * Default is 0.1.
   * @param seed Seed for random number generation. Default is 1.
   * @param iterations Number of iterations to apply meandering. Default is 1.
   * @param edge_divisions Number of sub-divisions of each edge. Default is 10.
   */
  void meanderize(float ratio,
                  float noise_ratio = 0.1f,
                  uint  seed = 1,
                  int   iterations = 1,
                  int   edge_divisions = 10);

  /**
   * @brief Reorder points using a nearest neighbor search.
   *
   * This method reorders the points in the path to minimize the total distance
   * by performing a nearest neighbor search starting from the specified
   * `start_index`. This approach is useful for optimizing the path or improving
   * its order.
   *
   * @param start_index Index of the starting point for the nearest neighbor
   * search. Default is 0.
   */
  void reorder_nns(int start_index = 0);

  /**
   * @brief Resample the path to achieve an approximately constant distance
   * between points.
   *
   * This method adjusts the points in the path to ensure that the distance
   * between each consecutive point is approximately equal to the specified
   * `delta`. This is useful for creating a path with evenly spaced points.
   *
   * @param delta Target distance between consecutive points.
   */
  void resample(float delta);

  /**
   * @brief Resample the path to achieve fairly uniform distance between
   * consecutive points.
   *
   * This method adjusts the path so that the distance between each consecutive
   * point is as uniform as possible. It redistributes the points to ensure more
   * even spacing along the path.
   */
  void resample_uniform();

  /**
   * @brief Reverse the order of points in the path.
   *
   * This method reverses the sequence of points in the path, which can be
   * useful for various applications, such as changing the direction of the path
   * traversal.
   */
  void reverse();

  /**
   * @brief Return the angle of the closest edge to the point (x, y), assuming a
   * closed path.
   *
   * This method calculates the angle of the edge closest to the specified point
   * (x, y) on a path that is closed. The angle is returned in radians. The path
   * is assumed to form a continuous loop, and the closest edge is determined
   * accordingly.
   *
   * **Example**
   * @include ex_path_sdf.cpp
   *
   * **Result**
   * @image html ex_path_sdf0.png
   * @image html ex_path_sdf1.png
   * @image html ex_path_sdf2.png
   *
   * @param x x coordinate of the point.
   * @param y y coordinate of the point.
   * @return float Angle of the closest edge in radians.
   */
  float sdf_angle_closed(float x, float y);

  /**
   * @brief Return the angle of the closest edge to the point (x, y), assuming
   * an open path.
   *
   * This method calculates the angle of the edge closest to the specified point
   * (x, y) on a path that is open. The angle is returned in radians. The path
   * is assumed to have distinct start and end points, and the closest edge is
   * determined based on this open structure.
   *
   * **Example**
   * @include ex_path_sdf.cpp
   *
   * **Result**
   * @image html ex_path_sdf0.png
   * @image html ex_path_sdf1.png
   * @image html ex_path_sdf2.png
   *
   * @param x x coordinate of the point.
   * @param y y coordinate of the point.
   * @return float Angle of the closest edge in radians.
   */
  float sdf_angle_open(float x, float y);

  /**
   * @brief Return the signed distance function value at (x, y), assuming a
   * closed path.
   *
   * This method computes the signed distance from the point (x, y) to the
   * nearest edge of a closed path. The distance is signed, meaning it indicates
   * whether the point is inside or outside the path, with negative values
   * typically indicating that the point is inside the path.
   *
   * **Example**
   * @include ex_path_sdf.cpp
   *
   * **Result**
   * @image html ex_path_sdf0.png
   * @image html ex_path_sdf1.png
   * @image html ex_path_sdf2.png
   *
   * @param x x coordinate of the point.
   * @param y y coordinate of the point.
   * @return float Signed distance to the nearest edge.
   */
  float sdf_closed(float x, float y);

  /**
   * @brief Return the elevation value at (x, y) away from the path based on a
   * downslope `slope`, assuming a closed path.
   *
   * This method calculates the elevation value at a point (x, y) based on its
   * distance from the closed path, adjusted by a downslope factor. The
   * downslope determines how quickly the elevation decreases as you move away
   * from the path.
   *
   * **Example**
   * @include ex_path_sdf.cpp
   *
   * **Result**
   * @image html ex_path_sdf0.png
   * @image html ex_path_sdf1.png
   * @image html ex_path_sdf2.png
   *
   * @param x x coordinate of the point.
   * @param y y coordinate of the point.
   * @param slope Downslope factor influencing the elevation decrease.
   * @return float Adjusted elevation value based on the downslope.
   */
  float sdf_elevation_closed(float x, float y, float slope);

  /**
   * @brief Return the elevation value at (x, y) away from the path based on a
   * downslope `slope`, assuming an open path.
   *
   * This method calculates the elevation value at a point (x, y) considering
   * its distance from an open path and adjusting it based on a downslope
   * factor. The downslope determines how the elevation decreases as you move
   * away from the path.
   *
   * **Example**
   * @include ex_path_sdf.cpp
   *
   * **Result**
   * @image html ex_path_sdf0.png
   * @image html ex_path_sdf1.png
   * @image html ex_path_sdf2.png
   *
   * @param x x coordinate of the point.
   * @param y y coordinate of the point.
   * @param slope Downslope factor affecting the elevation decrease.
   * @return float Adjusted elevation value based on the downslope.
   */
  float sdf_elevation_open(float x, float y, float slope);

  /**
   * @brief Return the value of the signed distance function at (x, y), assuming
   * an open path.
   *
   * This method computes the signed distance from the point (x, y) to the
   * nearest edge of an open path. The signed distance indicates how far the
   * point is from the path, with positive values typically representing the
   * outside of the path and negative values indicating the inside.
   *
   * **Example**
   * @include ex_path_sdf.cpp
   *
   * **Result**
   * @image html ex_path_sdf0.png
   * @image html ex_path_sdf1.png
   * @image html ex_path_sdf2.png
   *
   * @param x x coordinate of the point.
   * @param y y coordinate of the point.
   * @return float Signed distance to the nearest edge of the open path.
   */
  float sdf_open(float x, float y);

  /**
   * @brief Subsample the path by keeping only every n-th point.
   *
   * This method reduces the number of points in the path by retaining only
   * every 'step'-th point, effectively subsampling the path. This can be useful
   * for simplifying the path or reducing data size.
   *
   * @param step The interval of points to keep. For example, a step of 2 will
   * keep every second point.
   */
  void subsample(int step);

  /**
   * @brief Project path points to an array.
   *
   * This method projects the points of the path onto a 2D array, filling the
   * array based on the path's points. Optionally, the contour of the path can
   * be filled using flood fill if the `filled` parameter is set to true.
   *
   * **Example**
   * @include ex_path_to_array.cpp
   *
   * @param array The array to which the path points will be projected.
   * @param bbox Bounding box defining the domain of the array.
   * @param filled Boolean flag indicating whether to perform flood filling of
   * the path's contour.
   */
  void to_array(Array &array, Vec4<float> bbox, bool filled = false);

  /**
   * @brief Return an array filled with the signed distance function to the
   * path.
   *
   * This method computes the signed distance function (SDF) from the path and
   * fills an output array with the calculated values. The SDF represents the
   * distance from each point in the array to the nearest point on the path,
   * with positive values indicating distances outside the path and negative
   * values indicating distances inside the path.
   *
   * **Example**
   * @include ex_path_sdf.cpp
   *
   * **Result**
   * @image html ex_path_sdf.png
   *
   * @param shape Shape of the output array, defining its dimensions.
   * @param bbox Bounding box specifying the region to consider for the SDF
   * calculation.
   * @param p_noise_x Optional reference to an array of noise values in the
   * x-direction used for domain warping. If not provided, no noise is applied.
   * @param p_noise_y Optional reference to an array of noise values in the
   * y-direction used for domain warping. If not provided, no noise is applied.
   * @param bbox_array Bounding box of the destination array, used to map the
   * output array coordinates to the path coordinates.
   * @return Array The resulting array filled with the signed distance function
   * values.
   */
  Array to_array_sdf(Vec2<int>   shape,
                     Vec4<float> bbox,
                     Array      *p_noise_x = nullptr,
                     Array      *p_noise_y = nullptr,
                     Vec4<float> bbox_array = {0.f, 1.f, 0.f, 1.f});

  /**
   * @brief Export path as PNG image file.
   *
   * This method generates a PNG image representing the path and saves it to the
   * specified file. The resolution of the image can be adjusted using the
   * `shape` parameter.
   *
   * **Example**
   * @include ex_path_to_png.cpp
   *
   * @param fname The filename for the output PNG image.
   * @param shape Resolution of the image, specified as width and height.
   */
  void to_png(std::string fname, Vec2<int> shape = {512, 512});
};

/**
 * @brief Dig a path on a heightmap.
 *
 * This function modifies a heightmap array by "digging" a path into it based on
 * the provided path. The path is represented by a `Path` object, and the
 * function adjusts the height values in the `z` array to create the appearance
 * of a dug path. The width, border decay, and flattening radius parameters
 * control the characteristics of the dig. Optionally, the path can be forced to
 * have a monotonically decreasing elevation.
 *
 * **Example**
 * @include ex_dig_path.cpp
 *
 * **Result**
 * @image html ex_dig_path.png
 *
 * @param z Input array representing the heightmap to be modified.
 * @param path The path to be dug into the heightmap. The path will be processed
 * to create the dig effect.
 * @param width Radius of the path width in pixels. This determines how wide the
 * dug path will be.
 * @param decay Radius of the path border decay in pixels. This controls how
 * quickly the effect of the path fades out towards the edges.
 * @param flattening_radius Radius used to flatten the elevation of the path,
 * creating a smooth transition. This is measured in pixels.
 * @param force_downhill If `true`, the path's elevation will be forced to
 * decrease monotonically, creating a downhill effect.
 * @param bbox Bounding box specifying the region of the heightmap to consider
 * for the digging operation. It defines the area where the path is applied.
 * @param depth Optional depth parameter to specify the maximum depth of the
 * dig. If not provided, the default depth of 0.f is used.
 */
void dig_path(Array      &z,
              Path       &path,
              int         width = 1,
              int         decay = 2,
              int         flattening_radius = 16,
              bool        force_downhill = false,
              Vec4<float> bbox = {0.f, 1.f, 0.f, 1.f},
              float       depth = 0.f);

} // namespace hmap