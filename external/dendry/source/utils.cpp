#include "utils.h"

float cubic_interpolate(float p0, float p1, float p2, float p3, float t)
{
	assert(0.f <= t && t <= 1.f);

	return p1 + 0.5 * t * (p2 - p0 + t * (2.0 * p0 - 5.0 * p1 + 4.0 * p2 - p3 + t * (3.0 * (p1 - p2) + p3 - p0)));
}

float cubic_interpolate(const std::array<float, 4>& p, float t)
{
	assert(0.f <= t && t <= 1.f);

	return cubic_interpolate(p[0], p[1], p[2], p[3], t);
}

float bi_cubic_interpolate(const std::array<std::array<float, 4>, 4>& p, float u, float v)
{
	assert(0.f <= u && u <= 1.f);
	assert(0.f <= v && v <= 1.f);

	std::array<float, 4> temp{};
	for (unsigned int i = 0; i < 4; i++)
	{
		temp[i] = cubic_interpolate(p[i], v);
	}
	return cubic_interpolate(temp, u);
}

float matlab_jet_base(float val)
{
	if (val <= 0.125) {
		return 0.f;
	}
	else if (val <= 0.375) {
		return remap_clamp(val, 0.125f, 0.375f, 0.f, 1.f);
	}
	else if (val <= 0.625) {
		return 1.f;
	}
	else if (val <= 0.875) {
		return remap_clamp(val, 0.625f, 0.875f, 1.f, 0.f);
	}
	else {
		return 0.f;
	}
}

std::array<float, 3> matlab_jet(float u)
{
	const float r = matlab_jet_base(u - 0.25);
	const float g = matlab_jet_base(u);
	const float b = matlab_jet_base(u + 0.25);

	return { { r, g, b } };
}
