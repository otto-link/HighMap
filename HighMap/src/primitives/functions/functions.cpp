/* Copyright (c) 2023 Otto Link. Distributed under the terms of the GNU General
 * Public License. The full license is in the file LICENSE, distributed with
 * this software. */
#include "highmap/functions.hpp"
#include "highmap/math.hpp"

namespace hmap
{

//----------------------------------------------------------------------
// base class
//----------------------------------------------------------------------

HMAP_FCT_XY_TYPE Function::get_delegate() const
{
  return this->delegate;
}

float Function::get_value(float x, float y, float ctrl_param) const
{
  return this->delegate(x, y, ctrl_param);
}

void Function::set_delegate(HMAP_FCT_XY_TYPE new_delegate)
{
  this->delegate = std::move(new_delegate);
}

//----------------------------------------------------------------------
// derived from Function class
//----------------------------------------------------------------------

ArrayFunction::ArrayFunction(hmap::Array array, Vec2<float> kw, bool periodic)
    : Function(), kw(kw), array(array)
{
  if (periodic)
    this->set_delegate(
        [this](float x, float y, float)
        {
          float xp = 0.5f * this->kw.x * x;
          float yp = 0.5f * this->kw.y * y;

          xp = 2.f * std::abs(xp - int(xp));
          yp = 2.f * std::abs(yp - int(yp));

          xp = xp < 1.f ? xp : 2.f - xp;
          yp = yp < 1.f ? yp : 2.f - yp;

          smoothstep5(xp);
          smoothstep5(yp);

          float xg = xp * (this->array.shape.x - 1);
          float yg = yp * (this->array.shape.y - 1);
          int   i = (int)xg;
          int   j = (int)yg;
          return this->array.get_value_bilinear_at(i, j, xg - i, yg - j);
        });
  else
    this->set_delegate(
        [this](float x, float y, float)
        {
          float xp = this->kw.x * x;
          float yp = this->kw.y * y;

          xp = xp < 0.f ? 0.f : (xp >= 1.f ? 1.f : xp - int(xp));
          yp = yp < 0.f ? 0.f : (yp >= 1.f ? 1.f : yp - int(yp));

          float xg = xp * (this->array.shape.x - 1);
          float yg = yp * (this->array.shape.y - 1);
          int   i = (int)xg;
          int   j = (int)yg;

          return this->array.get_value_bilinear_at(i, j, xg - i, yg - j);
        });
}

BiquadFunction::BiquadFunction(float gain, Vec2<float> center)
    : Function(), gain(gain), center(center)
{
  this->set_delegate(
      [this](float x, float y, float ctrl_param)
      {
        float v = x * (x - 1.f) * y * (y - 1.f);
        v = std::clamp(v, 0.f, 1.f);
        return std::pow(v, 1.f / (this->gain * ctrl_param));
      });
}

BumpFunction::BumpFunction(float gain, Vec2<float> center)
    : Function(), gain(gain), center(center)
{
  this->set_delegate(
      [this](float x, float y, float ctrl_param)
      {
        float dx = x - this->center.x;
        float dy = y - this->center.y;

        float r2 = dx * dx + dy * dy;
        return r2 > 0.25f ? 0.f
                          : std::pow(std::exp(-1.f / (1.f - 4.f * r2)),
                                     1.f / (this->gain * ctrl_param));
      });
}

CraterFunction::CraterFunction(float       radius,
                               float       depth,
                               float       lip_decay,
                               float       lip_height_ratio,
                               Vec2<float> center)
    : Function(),
      radius(radius),
      depth(depth),
      lip_decay(lip_decay),
      lip_height_ratio(lip_height_ratio),
      center(center)
{
  this->set_delegate(
      [this](float x, float y, float ctrl_param)
      {
        float dx = x - this->center.x;
        float dy = y - this->center.y;
        float r = std::hypot(dx, dy);

        float value = std::min(
            r * r / (this->radius * this->radius),
            1.f + this->lip_height_ratio * ctrl_param *
                      std::exp(-(r - this->radius) / this->lip_decay));
        value -= 1.f;
        value *= this->depth;
        return value;
      });
}

DiskFunction::DiskFunction(float radius, float slope, Vec2<float> center)
    : Function(), radius(radius), slope(slope), center(center)
{
  this->set_delegate(
      [this](float x, float y, float ctrl_param)
      {
        float dx = x - this->center.x;
        float dy = y - this->center.y;
        float r = std::hypot(dx, dy);

        if (r < this->radius)
          return ctrl_param;
        else
        {
          float t = std::max(0.f, 1.f - this->slope * (r - this->radius));
          return ctrl_param * smoothstep3(t);
        }
      });
}

GaussianPulseFunction::GaussianPulseFunction(float sigma, Vec2<float> center)
    : Function(), center(center)
{
  this->set_sigma(sigma);
  this->set_delegate(
      [this](float x, float y, float ctrl_param)
      {
        float dx = x - this->center.x;
        float dy = y - this->center.y;
        float r2 = dx * dx + dy * dy;
        return std::exp(-0.5f * r2 * this->inv_sigma2 * ctrl_param);
      });
}

RectangleFunction::RectangleFunction(float       rx,
                                     float       ry,
                                     float       angle,
                                     float       slope,
                                     Vec2<float> center)
    : Function(), rx(rx), ry(ry), slope(slope), center(center)
{
  this->set_angle(angle);

  this->set_delegate(
      [this](float x, float y, float ctrl_param)
      {
        x = x - this->center.x;
        y = y - this->center.y;

        float xc = this->ca * x + this->sa * y;
        float yc = -this->sa * x + this->ca * y;

        xc = std::abs(xc);
        yc = std::abs(yc);

        float ax;
        float ay;

        if (xc < this->rx)
          ax = ctrl_param;
        else
        {
          float t = std::max(0.f, 1.f - this->slope * (xc - this->rx));
          ax = ctrl_param * smoothstep3(t);
        }

        if (yc < this->ry)
          ay = ctrl_param;
        else
        {
          float t = std::max(0.f, 1.f - this->slope * (yc - this->ry));
          ay = ctrl_param * smoothstep3(t);
        }

        return ax * ay;
      });
}

RiftFunction::RiftFunction(float       angle,
                           float       slope,
                           float       width,
                           bool        sharp_bottom,
                           Vec2<float> center)
    : Function(), slope(slope), width(width), center(center)
{
  this->set_angle(angle);

  if (sharp_bottom)
    this->set_delegate(
        [this](float x, float y, float ctrl_param)
        {
          float local_width = 0.5f * this->width * ctrl_param;

          float r = this->ca * (x - this->center.x) +
                    this->sa * (y - this->center.y);
          r = std::abs(r);

          if (r > local_width + 1.f / this->slope)
            return 1.f;
          else if (r < local_width)
            return 0.f;
          else
          {
            r = (r - local_width) * this->slope;
            return smoothstep3_upper(r);
          }
        });
  else
    this->set_delegate(
        [this](float x, float y, float ctrl_param)
        {
          float local_width = 0.5f * this->width * ctrl_param;

          float r = this->ca * (x - this->center.x) +
                    this->sa * (y - this->center.y);
          r = std::abs(r);

          if (r > local_width + 1.f / this->slope)
            return 1.f;
          else if (r < local_width)
            return 0.f;
          else
          {
            r = (r - local_width) * this->slope;
            return smoothstep3(r);
          }
        });
}

SlopeFunction::SlopeFunction(float angle, float slope, Vec2<float> center)
    : Function(), slope(slope), center(center)
{
  this->set_angle(angle);

  this->set_delegate(
      [this](float x, float y, float ctrl_param)
      {
        float r = this->ca * (x - this->center.x) +
                  this->sa * (y - this->center.y);
        return this->slope * ctrl_param * r;
      });
}

StepFunction::StepFunction(float angle, float slope, Vec2<float> center)
    : Function(), slope(slope), center(center)
{
  this->set_angle(angle);

  this->set_delegate(
      [this](float x, float y, float ctrl_param)
      {
        float local_slope = this->slope * ctrl_param;

        float r = this->ca * (x - this->center.x) +
                  this->sa * (y - this->center.y);
        float dt = 0.5f / local_slope;
        if (r > dt)
          return 1.f;
        else if (r > -dt)
        {
          r = local_slope * (r + dt);
          return smoothstep3(r);
        }
        else
          return 0.f;
      });
}

WaveDuneFunction::WaveDuneFunction(Vec2<float> kw,
                                   float       angle,
                                   float       xtop,
                                   float       xbottom,
                                   float       phase_shift)
    : Function(), kw(kw), xtop(xtop), xbottom(xbottom), phase_shift(phase_shift)
{
  this->set_angle(angle);

  this->set_delegate(
      [this](float x, float y, float)
      {
        float r = ca * this->kw.x * x + sa * this->kw.y * y;
        float xp = std::fmod(r + this->phase_shift +
                                 10.f * (this->kw.x + this->kw.y),
                             1.f);
        float yp = 0.f;

        if (xp < this->xtop)
        {
          float r = xp / this->xtop;
          yp = r * r * (3.f - 2.f * r);
        }
        else if (xp < this->xbottom)
        {
          float r = (xp - this->xbottom) / (this->xtop - this->xbottom);
          yp = r * r * (2.f - r);
        }
        return yp;
      });
}

WaveSineFunction::WaveSineFunction(Vec2<float> kw,
                                   float       angle,
                                   float       phase_shift)
    : Function(), kw(kw), phase_shift(phase_shift)
{
  this->set_angle(angle);

  this->set_delegate(
      [this](float x, float y, float)
      {
        float r = ca * this->kw.x * x + sa * this->kw.y * y;
        return std::cos(2.f * M_PI * r + this->phase_shift);
      });
}

WaveSquareFunction::WaveSquareFunction(Vec2<float> kw,
                                       float       angle,
                                       float       phase_shift)
    : Function(), kw(kw), phase_shift(phase_shift)
{
  this->set_angle(angle);

  this->set_delegate(
      [this](float x, float y, float)
      {
        float r = ca * this->kw.x * x + sa * this->kw.y * y + this->phase_shift;
        return r = 2.f * std::floor(r) - std::floor(2.f * r) + 1.f;
      });
}

WaveTriangularFunction::WaveTriangularFunction(Vec2<float> kw,
                                               float       angle,
                                               float       slant_ratio,
                                               float       phase_shift)
    : Function(), kw(kw), slant_ratio(slant_ratio), phase_shift(phase_shift)
{
  this->set_angle(angle);

  this->set_delegate(
      [this](float x, float y, float)
      {
        float r = ca * this->kw.x * x + sa * this->kw.y * y + this->phase_shift;

        r = r - std::floor(r);
        if (r < this->slant_ratio)
          r /= this->slant_ratio;
        else
          r = 1.f - (r - this->slant_ratio) / (1.f - this->slant_ratio);
        return r * r * (3.f - 2.f * r);
      });
}

} // namespace hmap
