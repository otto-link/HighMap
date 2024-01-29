#ifndef MATH3D_H
#define MATH3D_H

#include <cmath>
#include <array>

#include "utils.h"
#include "math2d.h"

struct Point3D;
struct Vec3D;
struct Segment3D;

/*
 * Point in a 3D Space
 */
struct Point3D
{
	float x;
	float y;
	float z;

	Point3D() : x(0.f), y(0.f), z(0.f) { }

	Point3D(float _x, float _y, float _z) : x(_x), y(_y), z(_z) { }

	explicit Point3D(const Point2D& point, float _z) : x(point.x), y(point.y), z(_z) { }

	// Unary Point operators
	Point3D& operator+=(const Point3D& p) { x += p.x; y += p.y; z += p.z; return *this; }
	Point3D& operator-=(const Point3D& p) { x -= p.x; y -= p.y; z -= p.z; return *this; }

	// Unary Vector operators
	Point3D& operator+=(const Vec3D& v);
	Point3D& operator-=(const Vec3D& v);

	// Scalar operators
	Point3D& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
	Point3D& operator/=(float s) { x /= s; y /= s; z /= s; return *this; }

	// Unary minus operator
	Point3D operator-() const { return { -x, -y, -z }; }
};

// Comparison operators
inline bool operator==(const Point3D& lhs, const Point3D& rhs) {
	return ((fabs(lhs.x - rhs.x) < EPS)
		 && (fabs(lhs.y - rhs.y) < EPS)
		 && (fabs(lhs.z - rhs.z) < EPS));
}

inline bool operator!=(const Point3D& lhs, const Point3D& rhs) {
	return !(lhs == rhs);
}

// Binary Point operators
inline Point3D operator+(const Point3D& a, const Point3D& b) {
	return Point3D(a) += b;
}

inline Point3D operator-(const Point3D& a, const Point3D& b) {
	return Point3D(a) -= b;
}

// Binary Vector operators
inline Point3D operator+(const Point3D& a, const Vec3D& v) {
	return Point3D(a) += v;
}

inline Point3D operator-(const Point3D& a, const Vec3D& v) {
	return Point3D(a) -= v;
}

// Binary scalar operators
inline Point3D operator*(const Point3D& a, float s) {
	return Point3D(a) *= s;
}

inline Point3D operator*(float s, const Point3D& a) {
	return Point3D(a) *= s;
}

inline Point3D operator/(const Point3D& a, float s) {
	return Point3D(a) /= s;
}

// Utility functions
inline float dist_sq(const Point3D& lhs, const Point3D& rhs) {
	return (lhs.x - rhs.x) * (lhs.x - rhs.x)
		 + (lhs.y - rhs.y) * (lhs.y - rhs.y)
		 + (lhs.z - rhs.z) * (lhs.z - rhs.z);
}

inline float dist(const Point3D& lhs, const Point3D& rhs) {
	return sqrt(dist_sq(lhs, rhs));
}

inline Point3D lerp(const Point3D& a, const Point3D& b, float t) {
	return {
		lerp(a.x, b.x, t),
		lerp(a.y, b.y, t),
		lerp(a.z, b.z, t)
	};
}

inline Point2D ProjectionZ(const Point3D& p) {
	return { p.x, p.y };
}

/*
 * Vector in a 3D Space
 */
struct Vec3D
{
	float x;
	float y;
	float z;

	Vec3D() : x(0.f), y(0.f), z(0.f) { }

	Vec3D(float _x, float _y, float _z) : x(_x), y(_y), z(_z) { }

	explicit Vec3D(const Point3D& p) : x(p.x), y(p.y), z(p.z) { }

	explicit Vec3D(const Point3D& a, const Point3D& b) : x(b.x - a.x), y(b.y - a.y), z(b.z - a.z) { }

	explicit Vec3D(const Vec2D& vec, float _z) : x(vec.x), y(vec.y), z(_z) { }

	// Unary Point operators
	Vec3D& operator+=(const Vec3D& v) { x += v.x; y += v.y; z += v.z; return *this; }
	Vec3D& operator-=(const Vec3D& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }

	// Scalar operators
	Vec3D& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
	Vec3D& operator/=(float s) { x /= s; y /= s; z /= s; return *this; }

	// Unary minus operator
	Vec3D operator-() const { return Vec3D(-x, -y, -z); }
};

// Comparison operators
inline bool operator==(const Vec3D& lhs, const Vec3D& rhs) {
	return ((fabs(lhs.x - rhs.x) < EPS)
		 && (fabs(lhs.y - rhs.y) < EPS)
		 && (fabs(lhs.z - rhs.z) < EPS));
}

inline bool operator!=(const Vec3D& lhs, const Vec3D& rhs) {
	return !(lhs == rhs);
}

// Binary Vector operators
inline Vec3D operator+(const Vec3D& a, const Vec3D& b) {
	return Vec3D(a) += b;
}

inline Vec3D operator-(const Vec3D& a, const Vec3D& b) {
	return Vec3D(a) -= b;
}

// Binary scalar operators
inline Vec3D operator*(const Vec3D& a, float s) {
	return Vec3D(a) *= s;
}

inline Vec3D operator*(float s, const Vec3D& a) {
	return Vec3D(a) *= s;
}

inline Vec3D operator/(const Vec3D& a, float s) {
	return Vec3D(a) /= s;
}

// Utility functions
inline float norm_sq(const Vec3D& a) {
	return a.x * a.x + a.y * a.y + a.z * a.z;
}

inline float norm(const Vec3D& a) {
	return sqrt(norm_sq(a));
}

inline float dot(const Vec3D& a, const Vec3D& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vec3D cross(const Vec3D& a, const Vec3D& b) {
	return {
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	};
}

inline Vec3D normalized(const Vec3D& a) {
	const float n = norm(a);
	return { a.x / n, a.y / n, a.z / n };
}

inline Vec2D ProjectionZ(const Vec3D& v) {
	return { v.x, v.y };
}

inline float angle(const Vec3D& oa, const Vec3D& ob) {
	return acos(dot(oa, ob) / sqrt(norm_sq(oa) * norm_sq(ob)));
}

inline Vec3D rotate_axis(const Vec3D& v, const Vec3D& axis, float angle)
{
	assert(abs(norm_sq(axis) - 1.f) < 1e-6);

	const float sinAngle = sin(angle);
	const float cosAngle = cos(angle);
	const float oneMinusCosAngle = 1.f - cosAngle;

	const Vec3D rotMatrixRow0 = {
		axis.x * axis.x + cosAngle * (1 - axis.x * axis.x),
		axis.x * axis.y * oneMinusCosAngle - sinAngle * axis.z,
		axis.x * axis.z * oneMinusCosAngle + sinAngle * axis.y
	};

	const Vec3D rotMatrixRow1 = {
		axis.x * axis.y * oneMinusCosAngle + sinAngle * axis.z,
		axis.y * axis.y + cosAngle * (1 - axis.y * axis.y),
		axis.y * axis.z * oneMinusCosAngle - sinAngle * axis.x
	};

	const Vec3D rotMatrixRow2 = {
		axis.x * axis.z * oneMinusCosAngle - sinAngle * axis.y,
		axis.y * axis.z * oneMinusCosAngle + sinAngle * axis.x,
		axis.z * axis.z + cosAngle * (1 - axis.z * axis.z)
	};

	return {
		dot(v, rotMatrixRow0),
		dot(v, rotMatrixRow1),
		dot(v, rotMatrixRow2)
	};
}

struct Segment3D
{
	Point3D a;
	Point3D b;

	Segment3D() = default;

	explicit Segment3D(const Point3D& _a, const Point3D& _b) : a(_a), b(_b) { }
};

// Utility functions
inline float length_sq(const Segment3D& s) {
	return dist_sq(s.a, s.b);
}

inline float length(const Segment3D& s) {
	return dist(s.a, s.b);
}

inline Point3D lerp(const Segment3D& s, float t) {
	return lerp(s.a, s.b, t);
}

inline Point3D MidPoint(const Segment3D& s) {
	return {
		(s.a.x + s.b.x) / 2.f,
		(s.a.y + s.b.y) / 2.f,
		(s.a.z + s.b.z) / 2.f
	};
}

template <size_t N>
std::array<Point3D, N> SubdivideInPoints(const Segment3D& s)
{
	std::array<Point3D, N> points;

	for (int n = 0; n < points.size(); n++)
	{
		const float t = float(n + 1) / (N + 1);
		points[n] = lerp(s.a, s.b, t);
	}

	return points;
}

template <size_t N>
std::array<Segment3D, N> SubdivideInSegments(const Segment3D& s)
{
	static_assert(N > 0, "Segment should be divided in at least one part.");

	std::array<Segment3D, N> segments;

	segments.front().a = s.a;
	for (int n = 0; n < segments.size() - 1; n++)
	{
		const float t = float(n + 1) / N;
		const Point3D point = lerp(s.a, s.b, t);
		segments[n].b = point;
		segments[n + 1].a = point;
	}
	segments.back().b = s.b;

	return segments;
}

inline Segment2D ProjectionZ(const Segment3D& s) {
	return { ProjectionZ(s.a), ProjectionZ(s.b) };
}

#endif // MATH3D_H
