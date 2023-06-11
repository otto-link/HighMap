/**
 * @file lut_function.hpp
 * @author Otto Link (otto.link.bv@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-06-11
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <functional>
#include <vector>

#include "macrologger.h"

#include "highmap/op.hpp"

namespace hmap{

class LUTFunction1D
{
public:
  LUTFunction1D(std::function<float(float)> function,
                float lbound,
                float ubound,
                int   nvalues)
      : function(function), lbound(lbound), ubound(ubound), nvalues(nvalues)
  {
    std::vector<float> xv = linspace(lbound, ubound, nvalues);
    values.resize(nvalues);
    
    for (int k = 0; k < nvalues; k++)
      {
	values[k] = function(xv[k]);
      }

    // factors to retrieve the index
    a = 1.f / (ubound - lbound);
    b = -lbound / (ubound - lbound);
  }; 

  inline float value_linear(float x)
  {
    float kf = (float)(nvalues - 1) * (a * x + b);
    int k = (int) kf;
    float u = kf - (float) k;
    return (1.f - u) * values[k] + u * values[k + 1];
  }
  
  inline float value_nearest(float x)
  {
    int k = (int) ((float)(nvalues - 1) * (a * x + b));
    return values[k];
  }
  
private:
  std::function<float(float)> function;
  float lbound;
  float ubound;
  float a;
  float b;
  int   nvalues;
  std::vector<float> values;
};

}
