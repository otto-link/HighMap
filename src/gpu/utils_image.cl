R""(
/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
void update_interp_param(float2 pos,
                         int2   shape,
                         int   *i,
                         int   *j,
                         float *u,
                         float *v)
{
  *i = (int)pos.x;
  *j = (int)pos.y;
  *u = pos.x - *i;
  *v = pos.y - *j;
}

float image_bilinear_interp(read_write image2d_t img,
                            int                  i,
                            int                  j,
                            float                u,
                            float                v)
{
  float a10 = read_imagef(img, (int2)(j, i + 1)).x -
              read_imagef(img, (int2)(j, i)).x;
  float a01 = read_imagef(img, (int2)(j + 1, i)).x -
              read_imagef(img, (int2)(j, i)).x;
  float a11 = read_imagef(img, (int2)(j + 1, i + 1)).x -
              read_imagef(img, (int2)(j, i + 1)).x -
              read_imagef(img, (int2)(j + 1, i)).x +
              read_imagef(img, (int2)(j, i)).x;

  return read_imagef(img, (int2)(j, i)).x + a10 * u + a01 * v + a11 * u * v;
}

float image_gradient_x_bilinear_interp(read_write image2d_t img,
                                       int                  i,
                                       int                  j,
                                       float                u,
                                       float                v)
{
  float f00 = read_imagef(img, (int2)(j, i)).x -
              read_imagef(img, (int2)(j, i - 1)).x;
  float f10 = read_imagef(img, (int2)(j, i + 1)).x -
              read_imagef(img, (int2)(j, i)).x;
  float f01 = read_imagef(img, (int2)(j + 1, i)).x -
              read_imagef(img, (int2)(j + 1, i - 1)).x;
  float f11 = read_imagef(img, (int2)(j + 1, i + 1)).x -
              read_imagef(img, (int2)(j + 1, i)).x;

  float a10 = f10 - f00;
  float a01 = f01 - f00;
  float a11 = f11 - f10 - f01 + f00;

  return f00 + a10 * u + a01 * v + a11 * u * v;
}

float image_gradient_x(read_write image2d_t img, int i, int j)
{
  return 0.5f * (read_imagef(img, (int2)(j, i + 1)).x -
                 read_imagef(img, (int2)(j, i - 1)).x);
}

float image_gradient_y(read_write image2d_t img, int i, int j)
{
  return 0.5f * (read_imagef(img, (int2)(j + 1, i)).x -
                 read_imagef(img, (int2)(j - 1, i)).x);
}

float image_gradient_x_prewitt(read_write image2d_t img, int i, int j)
{
  float dx = 0.166667f * read_imagef(img, (int2)(j, i + 1)).x -
             0.166667f * read_imagef(img, (int2)(j, i - 1)).x;
  dx += 0.166667f * read_imagef(img, (int2)(j - 1, i + 1)).x -
        0.166667f * read_imagef(img, (int2)(j - 1, i - 1)).x;
  dx += 0.166667f * read_imagef(img, (int2)(j + 1, i + 1)).x -
        0.166667f * read_imagef(img, (int2)(j + 1, i - 1)).x;
  return dx;
}

float image_gradient_y_prewitt(read_write image2d_t img, int i, int j)
{
  float dy = 0.166667f * read_imagef(img, (int2)(j + 1, i)).x -
             0.166667f * read_imagef(img, (int2)(j - 1, i)).x;
  dy += 0.166667f * read_imagef(img, (int2)(j + 1, i - 1)).x -
        0.166667f * read_imagef(img, (int2)(j - 1, i - 1)).x;
  dy += 0.166667f * read_imagef(img, (int2)(j + 1, i + 1)).x -
        0.166667f * read_imagef(img, (int2)(j - 1, i + 1)).x;
  return dy;
}

float image_gradient_x_scharr(read_write image2d_t img, int i, int j)
{
  float dx = 0.3125f * read_imagef(img, (int2)(j, i + 1)).x -
             0.3125f * read_imagef(img, (int2)(j, i - 1)).x;
  dx += 0.09375f * read_imagef(img, (int2)(j - 1, i + 1)).x -
        0.09375f * read_imagef(img, (int2)(j - 1, i - 1)).x;
  dx += 0.09375f * read_imagef(img, (int2)(j + 1, i + 1)).x -
        0.09375f * read_imagef(img, (int2)(j + 1, i - 1)).x;
  return dx;
}

float image_gradient_y_scharr(read_write image2d_t img, int i, int j)
{
  float dy = 0.3125f * read_imagef(img, (int2)(j + 1, i)).x -
             0.3125f * read_imagef(img, (int2)(j - 1, i)).x;
  dy += 0.09375f * read_imagef(img, (int2)(j + 1, i - 1)).x -
        0.09375f * read_imagef(img, (int2)(j - 1, i - 1)).x;
  dy += 0.09375f * read_imagef(img, (int2)(j + 1, i + 1)).x -
        0.09375f * read_imagef(img, (int2)(j - 1, i + 1)).x;
  return dy;
}

float image_gradient_x_sobel(read_write image2d_t img, int i, int j)
{
  float dx = 0.25f * read_imagef(img, (int2)(j, i + 1)).x -
             0.25f * read_imagef(img, (int2)(j, i - 1)).x;
  dx += 0.125f * read_imagef(img, (int2)(j - 1, i + 1)).x -
        0.125f * read_imagef(img, (int2)(j - 1, i - 1)).x;
  dx += 0.125f * read_imagef(img, (int2)(j + 1, i + 1)).x -
        0.125f * read_imagef(img, (int2)(j + 1, i - 1)).x;
  return dx;
}

float image_gradient_y_sobel(read_write image2d_t img, int i, int j)
{
  float dy = 0.25f * read_imagef(img, (int2)(j + 1, i)).x -
             0.25f * read_imagef(img, (int2)(j - 1, i)).x;
  dy += 0.125f * read_imagef(img, (int2)(j + 1, i - 1)).x -
        0.125f * read_imagef(img, (int2)(j - 1, i - 1)).x;
  dy += 0.125f * read_imagef(img, (int2)(j + 1, i + 1)).x -
        0.125f * read_imagef(img, (int2)(j - 1, i + 1)).x;
  return dy;
}

float image_gradient_y_bilinear_interp(read_write image2d_t img,
                                       int                  i,
                                       int                  j,
                                       float                u,
                                       float                v)
{
  float f00 = read_imagef(img, (int2)(j, i)).x -
              read_imagef(img, (int2)(j - 1, i)).x;
  float f10 = read_imagef(img, (int2)(j, i + 1)).x -
              read_imagef(img, (int2)(j - 1, i + 1)).x;
  float f01 = read_imagef(img, (int2)(j + 1, i)).x -
              read_imagef(img, (int2)(j, i)).x;
  float f11 = read_imagef(img, (int2)(j + 1, i + 1)).x -
              read_imagef(img, (int2)(j, i + 1)).x;

  float a10 = f10 - f00;
  float a01 = f01 - f00;
  float a11 = f11 - f10 - f01 + f00;

  return f00 + a10 * u + a01 * v + a11 * u * v;
}

void image_depose_amount_bilinear(read_write image2d_t img,
                                  int                  i,
                                  int                  j,
                                  float                u,
                                  float                v,
                                  float                amount)
{
  float f00 = read_imagef(img, (int2)(j, i)).x;
  float f10 = read_imagef(img, (int2)(j, i + 1)).x;
  float f01 = read_imagef(img, (int2)(j + 1, i)).x;
  float f11 = read_imagef(img, (int2)(j + 1, i + 1)).x;

  write_imagef(img, (int2)(j, i), f00 + amount * (1 - u) * (1 - v));
  write_imagef(img, (int2)(j, i + 1), f10 + amount * u * (1 - v));
  write_imagef(img, (int2)(j + 1, i), f01 + amount * (1 - u) * v);
  write_imagef(img, (int2)(j + 1, i + 1), f11 + amount * u * v);
}
)""
