#ifndef VECTOR_H_
#define VECTOR_H_

#include <cmath>
#include <cstdint>

static const int32_t FIXED_ONE = 1<<16;

class Vector {

public:
	typedef int32_t T;

	Vector(): x(0), y(0) {}
	Vector(T x_, T y_): x(x_), y(y_) {}

	inline Vector operator + (const Vector &v) const {
		return Vector(x+v.x, y+v.y);
	}
	inline Vector operator - (const Vector &v) const {
		return Vector(x-v.x, y-v.y);
	}
	inline Vector operator - () const {
		return Vector(-x, -y);
	}
	inline Vector operator * (T a) const {
		return Vector(x*a, y*a);
	}
	inline Vector operator / (T a) const {
		return Vector(x/a, y/a);
	}

	inline const Vector &operator += (const Vector &v) {
		x += v.x; y += v.y; return *this;
	}
	inline const Vector &operator -= (const Vector &v) {
		x -= v.x; y -= v.y; return *this;
	}
	inline const Vector &operator *= (T a) {
		x *= a; y *= a; return *this;
	}
	inline const Vector &operator /= (T a) {
		x /= a; y /= a; return *this;
	}

	inline T size() const {
		//TODO: without doubles
		return (T)sqrt((double)((int64_t)x*x + (int64_t)y*y));
	}

	inline void normalize(T targetSize) {
		T s = size();
		T nx = (T)((int64_t)x*targetSize/s);
		T ny = (T)((int64_t)y*targetSize/s);
		x = nx; y = ny;
	}

	static Vector polar(uint16_t dir, T size_) {
		//TODO: without doubles
		double d = dir/18000.0*M_PI;
		return Vector((T)(cos(d)*size_), (T)(sin(d)*size_));
	}

	T x, y;

};

inline Vector operator * (Vector::T a, const Vector &v) {
	return v*a;
}

typedef Vector Point;

#endif
