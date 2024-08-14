/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include <cstdint>
#include <vector>

#include "macrologger.h"

#include "highmap/array.hpp"
#include "highmap/io.hpp"

namespace hmap
{

std::vector<uint8_t> read_png_grayscale_8bit(std::string fname)
{
  FILE *fp = fopen(fname.c_str(), "rb");
  if (!fp)
    LOG_ERROR("Error opening file for writing: %s", fname.c_str());

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                           nullptr,
                                           nullptr,
                                           nullptr);
  if (!png)
  {
    LOG_ERROR("Error creating PNG write structure");
    fclose(fp);
  }

  png_infop info = png_create_info_struct(png);
  if (!info)
  {
    LOG_ERROR("Error creating PNG info structure");
    png_destroy_write_struct(&png, nullptr);
    fclose(fp);
  }

  png_init_io(png, fp);
  png_read_info(png, info);

  int width = png_get_image_width(png, info);
  int height = png_get_image_height(png, info);

  std::vector<png_bytep> row_pointers(height);
  for (int j = 0; j < height; j++)
    row_pointers[j] = (png_byte *)malloc(png_get_rowbytes(png, info));
  png_read_image(png, row_pointers.data());

  std::vector<uint8_t> img(width * height);

  for (int j = 0; j < height; j++)
    for (int i = 0; i < width; i++)
      img[j * width + i] = (uint8_t)row_pointers[j][i];

  png_destroy_read_struct(&png, &info, nullptr);
  fclose(fp);

  return img;
}

std::vector<uint16_t> read_png_grayscale_16bit(std::string fname)
{
  FILE *fp = fopen(fname.c_str(), "rb");
  if (!fp)
    LOG_ERROR("Error opening file for writing: %s", fname.c_str());

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                           nullptr,
                                           nullptr,
                                           nullptr);
  if (!png)
  {
    LOG_ERROR("Error creating PNG write structure");
    fclose(fp);
  }

  png_infop info = png_create_info_struct(png);
  if (!info)
  {
    LOG_ERROR("Error creating PNG info structure");
    png_destroy_write_struct(&png, nullptr);
    fclose(fp);
  }

  png_init_io(png, fp);
  png_read_info(png, info);

  int width = png_get_image_width(png, info);
  int height = png_get_image_height(png, info);

  std::vector<png_bytep> row_pointers(height);
  for (int j = 0; j < height; j++)
    row_pointers[j] = (png_byte *)malloc(png_get_rowbytes(png, info));
  png_read_image(png, row_pointers.data());

  // convert data
  std::vector<uint16_t> img(width * height);

  for (int j = 0; j < height; j++)
  {
    for (int i = 0; i < 2 * width; i += 2)
      img[j * width + i / 2] = (uint16_t)((row_pointers[j][i] << 8) |
                                          row_pointers[j][i + 1]);
  }

  png_destroy_read_struct(&png, &info, nullptr);
  fclose(fp);

  return img;
}

void read_png_header(std::string fname,
                     int        &width,
                     int        &height,
                     png_byte   &color_type,
                     png_byte   &bit_depth)
{
  FILE *fp = fopen(fname.c_str(), "rb");
  if (!fp)
    LOG_ERROR("Error opening file for writing: %s", fname.c_str());

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                           nullptr,
                                           nullptr,
                                           nullptr);
  if (!png)
  {
    LOG_ERROR("Error creating PNG write structure");
    fclose(fp);
  }

  png_infop info = png_create_info_struct(png);
  if (!info)
  {
    LOG_ERROR("Error creating PNG info structure");
    png_destroy_write_struct(&png, nullptr);
    fclose(fp);
  }

  png_init_io(png, fp);
  png_read_info(png, info);

  width = png_get_image_width(png, info);
  height = png_get_image_height(png, info);
  color_type = png_get_color_type(png, info);
  bit_depth = png_get_bit_depth(png, info);

  LOG_DEBUG("width: %d", width);
  LOG_DEBUG("height: %d", height);
  LOG_DEBUG("color_type: %d", color_type);
  LOG_DEBUG("bit_depth: %d", bit_depth);

  fclose(fp);
  png_destroy_read_struct(&png, &info, nullptr);
}

void write_png_grayscale_8bit(std::string           fname,
                              std::vector<uint8_t> &img,
                              Vec2<int>             shape)
{
  FILE *fp = fopen(fname.c_str(), "wb");
  if (!fp)
    LOG_ERROR("Error opening file for writing: %s", fname.c_str());

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                            nullptr,
                                            nullptr,
                                            nullptr);
  if (!png)
  {
    LOG_ERROR("Error creating PNG write structure");
    fclose(fp);
  }

  png_infop info = png_create_info_struct(png);
  if (!info)
  {
    LOG_ERROR("Error creating PNG info structure");
    png_destroy_write_struct(&png, nullptr);
    fclose(fp);
  }

  if (setjmp(png_jmpbuf(png)))
  {
    LOG_ERROR("Error during PNG creation");
    png_destroy_write_struct(&png, &info);
    fclose(fp);
  }

  // Initialize the PNG file
  png_init_io(png, fp);

  // Set the image properties
  png_set_IHDR(png,
               info,
               shape.x,
               shape.y,
               8,
               PNG_COLOR_TYPE_GRAY,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);

  // Write the PNG header
  png_write_info(png, info);
  png_set_swap(png);

  // Allocate memory for row pointers
  int        width = shape.x;
  int        height = shape.y;
  png_bytep *row_pointers = new png_bytep[height];
  for (int i = 0; i < height; ++i)
    row_pointers[i] = (png_bytep)&img[i * width];

  // Write the image data
  png_write_image(png, row_pointers);

  // End the PNG write process
  png_write_end(png, nullptr);

  // Clean up
  png_destroy_write_struct(&png, &info);
  fclose(fp);
  delete[] row_pointers;

  LOG_DEBUG("PNG file (8 bit grayscale) saved successfully: %s", fname.c_str());
}

void write_png_grayscale_16bit(std::string            fname,
                               std::vector<uint16_t> &img,
                               Vec2<int>              shape)
{
  FILE *fp = fopen(fname.c_str(), "wb");
  if (!fp)
    LOG_ERROR("Error opening file for writing: %s", fname.c_str());

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                            nullptr,
                                            nullptr,
                                            nullptr);
  if (!png)
  {
    LOG_ERROR("Error creating PNG write structure");
    fclose(fp);
  }

  png_infop info = png_create_info_struct(png);
  if (!info)
  {
    LOG_ERROR("Error creating PNG info structure");
    png_destroy_write_struct(&png, nullptr);
    fclose(fp);
  }

  if (setjmp(png_jmpbuf(png)))
  {
    LOG_ERROR("Error during PNG creation");
    png_destroy_write_struct(&png, &info);
    fclose(fp);
  }

  // Initialize the PNG file
  png_init_io(png, fp);

  // Set the image properties
  png_set_IHDR(png,
               info,
               shape.x,
               shape.y,
               16,
               PNG_COLOR_TYPE_GRAY,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);

  // Write the PNG header
  png_write_info(png, info);
  png_set_swap(png);

  // Allocate memory for row pointers
  int        width = shape.x;
  int        height = shape.y;
  png_bytep *row_pointers = new png_bytep[height];
  for (int i = 0; i < height; ++i)
    row_pointers[i] = (png_bytep)&img[i * width];

  // Write the image data
  png_write_image(png, row_pointers);

  // End the PNG write process
  png_write_end(png, nullptr);

  // Clean up
  png_destroy_write_struct(&png, &info);
  fclose(fp);
  delete[] row_pointers;

  LOG_DEBUG("PNG file (16 bit grayscale) saved successfully: %s",
            fname.c_str());
}

void write_png_rgb_8bit(std::string           fname,
                        std::vector<uint8_t> &img,
                        Vec2<int>             shape)
{
  FILE *fp = fopen(fname.c_str(), "wb");
  if (!fp)
    LOG_ERROR("Error opening file for writing: %s", fname.c_str());

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                            nullptr,
                                            nullptr,
                                            nullptr);
  if (!png)
  {
    LOG_ERROR("Error creating PNG write structure");
    fclose(fp);
  }

  png_infop info = png_create_info_struct(png);
  if (!info)
  {
    LOG_ERROR("Error creating PNG info structure");
    png_destroy_write_struct(&png, nullptr);
    fclose(fp);
  }

  if (setjmp(png_jmpbuf(png)))
  {
    LOG_ERROR("Error during PNG creation");
    png_destroy_write_struct(&png, &info);
    fclose(fp);
  }

  // Initialize the PNG file
  png_init_io(png, fp);

  // Set the image properties
  png_set_IHDR(png,
               info,
               shape.x,
               shape.y,
               8,
               PNG_COLOR_TYPE_RGB,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);

  // Write the PNG header
  png_write_info(png, info);
  png_set_swap(png);

  // Allocate memory for row pointers
  int        width = shape.x;
  int        height = shape.y;
  png_bytep *row_pointers = new png_bytep[height];
  for (int i = 0; i < height; ++i)
    row_pointers[i] = (png_bytep)&img[i * width * 3];

  // Write the image data
  png_write_image(png, row_pointers);

  // End the PNG write process
  png_write_end(png, nullptr);

  // Clean up
  png_destroy_write_struct(&png, &info);
  fclose(fp);
  delete[] row_pointers;

  LOG_DEBUG("PNG file (8 bit RGB) saved successfully: %s", fname.c_str());
}

void write_png_rgb_16bit(std::string            fname,
                         std::vector<uint16_t> &img,
                         Vec2<int>              shape)
{
  FILE *fp = fopen(fname.c_str(), "wb");
  if (!fp)
    LOG_ERROR("Error opening file for writing: %s", fname.c_str());

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                            nullptr,
                                            nullptr,
                                            nullptr);
  if (!png)
  {
    LOG_ERROR("Error creating PNG write structure");
    fclose(fp);
  }

  png_infop info = png_create_info_struct(png);
  if (!info)
  {
    LOG_ERROR("Error creating PNG info structure");
    png_destroy_write_struct(&png, nullptr);
    fclose(fp);
  }

  if (setjmp(png_jmpbuf(png)))
  {
    LOG_ERROR("Error during PNG creation");
    png_destroy_write_struct(&png, &info);
    fclose(fp);
  }

  // Initialize the PNG file
  png_init_io(png, fp);

  // Set the image properties
  png_set_IHDR(png,
               info,
               shape.x,
               shape.y,
               16,
               PNG_COLOR_TYPE_RGB,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);

  // Write the PNG header
  png_write_info(png, info);
  png_set_swap(png);

  // Allocate memory for row pointers
  int        width = shape.x;
  int        height = shape.y;
  png_bytep *row_pointers = new png_bytep[height];
  for (int i = 0; i < height; ++i)
    row_pointers[i] = (png_bytep)&img[i * width * 3];

  // Write the image data
  png_write_image(png, row_pointers);

  // End the PNG write process
  png_write_end(png, nullptr);

  // Clean up
  png_destroy_write_struct(&png, &info);
  fclose(fp);
  delete[] row_pointers;

  LOG_DEBUG("PNG file (16 bit RGB) saved successfully: %s", fname.c_str());
}

void write_png_rgba_8bit(std::string           fname,
                         std::vector<uint8_t> &img,
                         Vec2<int>             shape)
{
  FILE *fp = fopen(fname.c_str(), "wb");
  if (!fp)
    LOG_ERROR("Error opening file for writing: %s", fname.c_str());

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                            nullptr,
                                            nullptr,
                                            nullptr);
  if (!png)
  {
    LOG_ERROR("Error creating PNG write structure");
    fclose(fp);
  }

  png_infop info = png_create_info_struct(png);
  if (!info)
  {
    LOG_ERROR("Error creating PNG info structure");
    png_destroy_write_struct(&png, nullptr);
    fclose(fp);
  }

  if (setjmp(png_jmpbuf(png)))
  {
    LOG_ERROR("Error during PNG creation");
    png_destroy_write_struct(&png, &info);
    fclose(fp);
  }

  // Initialize the PNG file
  png_init_io(png, fp);

  // Set the image properties
  png_set_IHDR(png,
               info,
               shape.x,
               shape.y,
               8,
               PNG_COLOR_TYPE_RGBA,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);

  // Write the PNG header
  png_write_info(png, info);
  png_set_swap(png);

  // Allocate memory for row pointers
  int        width = shape.x;
  int        height = shape.y;
  png_bytep *row_pointers = new png_bytep[height];
  for (int i = 0; i < height; ++i)
    row_pointers[i] = (png_bytep)&img[i * width * 4];

  // Write the image data
  png_write_image(png, row_pointers);

  // End the PNG write process
  png_write_end(png, nullptr);

  // Clean up
  png_destroy_write_struct(&png, &info);
  fclose(fp);
  delete[] row_pointers;

  LOG_DEBUG("PNG file (8 bit RGB) saved successfully: %s", fname.c_str());
}

void write_png_rgba_16bit(std::string            fname,
                          std::vector<uint16_t> &img,
                          Vec2<int>              shape)
{
  FILE *fp = fopen(fname.c_str(), "wb");
  if (!fp)
    LOG_ERROR("Error opening file for writing: %s", fname.c_str());

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                            nullptr,
                                            nullptr,
                                            nullptr);
  if (!png)
  {
    LOG_ERROR("Error creating PNG write structure");
    fclose(fp);
  }

  png_infop info = png_create_info_struct(png);
  if (!info)
  {
    LOG_ERROR("Error creating PNG info structure");
    png_destroy_write_struct(&png, nullptr);
    fclose(fp);
  }

  if (setjmp(png_jmpbuf(png)))
  {
    LOG_ERROR("Error during PNG creation");
    png_destroy_write_struct(&png, &info);
    fclose(fp);
  }

  // Initialize the PNG file
  png_init_io(png, fp);

  // Set the image properties
  png_set_IHDR(png,
               info,
               shape.x,
               shape.y,
               16,
               PNG_COLOR_TYPE_RGBA,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);

  // Write the PNG header
  png_write_info(png, info);
  png_set_swap(png);

  // Allocate memory for row pointers
  int        width = shape.x;
  int        height = shape.y;
  png_bytep *row_pointers = new png_bytep[height];
  for (int i = 0; i < height; ++i)
    row_pointers[i] = (png_bytep)&img[i * width * 4];

  // Write the image data
  png_write_image(png, row_pointers);

  // End the PNG write process
  png_write_end(png, nullptr);

  // Clean up
  png_destroy_write_struct(&png, &info);
  fclose(fp);
  delete[] row_pointers;

  LOG_DEBUG("PNG file (16 bit RGBA) saved successfully: %s", fname.c_str());
}

} // namespace hmap
