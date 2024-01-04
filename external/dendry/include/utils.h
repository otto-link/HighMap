#ifndef UTILS_H
#define UTILS_H

#include <cassert>
#include <array>

template<typename T>
T remap(const T& x, const T& in_start, const T& in_end, const T& out_start, const T& out_end)
{
	assert(in_start != in_end);
	
	return out_start + (out_end - out_start) * (x - in_start) / (in_end - in_start);
}

template<typename T>
T remap_clamp(const T& x, const T& in_start, const T& in_end, const T& out_start, const T& out_end)
{
	assert(in_start != in_end);

	if (x < in_start)
	{
		return out_start;
	}
	else if (x > in_end)
	{
		return out_end;
	}
	else
	{
		return out_start + (out_end - out_start) * (x - in_start) / (in_end - in_start);
	}
}

template<typename T>
T lerp(const T& a, const T& b, const T& x)
{
	// FMA friendly
	return x * b + (a - a * x);
}

template<typename T>
T lerp_clamp(const T& a, const T& b, const T& x)
{
	if (x < 0.f)
	{
		return a;
	}
	else if (x > 1.f)
	{
		return b;
	}
	else
	{
		return lerp(a, b, x);
	}
}

template<typename T>
T smoother(T x)
{
	return x * x * x * (x * (x * 6.0 - 15.0) + 10.f);
}

template<typename T>
T robust_mod(T a, T b)
{
	T ret = a % b;

	if (ret < 0)
	{
		ret += b;
	}

	return ret;
}

template<typename T>
T smootherstep(const T& edge0, const T& edge1, const T& x)
{
	// Scale, and clamp x to 0..1 range
	const T t = remap_clamp(x, edge0, edge1, 0.f, 1.f);
	// Evaluate polynomial
	return smoother(t);
}

template<typename T>
T WyvillGalinFunction(const T& distance, const T& R, const T& N)
{
	T alpha = 0.f;

	if (distance < R)
	{
		alpha = pow(1 - (distance / R) * (distance / R), N);
	}

	return alpha;
}

float cubic_interpolate(float p0, float p1, float p2, float p3, float t);

float cubic_interpolate(const std::array<float, 4>& p, float t);

float bi_cubic_interpolate(const std::array<std::array<float, 4>, 4>& p, float u, float v);

// Equivalent of the Jet coloring in Matlab.
std::array<float, 3> matlab_jet(float u);

#endif // UTILS_H