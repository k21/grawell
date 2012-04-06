#ifndef VECTOR_H_
#define VECTOR_H_

#include <cmath>

class Vector {

public:
	Vector(): x(0), y(0) {}
	Vector(double x_, double y_): x(x_), y(y_) {}

	inline Vector operator + (const Vector &v) const {
		return Vector(x+v.x, y+v.y);
	}
	inline Vector operator - (const Vector &v) const {
		return Vector(x-v.x, y-v.y);
	}
	inline Vector operator - () const {
		return Vector(-x, -y);
	}
	inline Vector operator * (double a) const {
		return Vector(x*a, y*a);
	}
	inline Vector operator / (double a) const {
		return Vector(x/a, y/a);
	}

	inline const Vector &operator += (const Vector &v) {
		x += v.x; y += v.y; return *this;
	}
	inline const Vector &operator -= (const Vector &v) {
		x -= v.x; y -= v.y; return *this;
	}
	inline const Vector &operator *= (double a) {
		x *= a; y *= a; return *this;
	}
	inline const Vector &operator /= (double a) {
		x /= a; y /= a; return *this;
	}

	inline double sizeSqr() const {
		return x*x + y*y;
	}
	inline double size() const {
		return sqrt(x*x + y*y);
	}
	inline double dir() const {
		return atan2(y, x);
	}

	inline double dot(const Vector &v) const {
		return x*v.x + y*v.y;
	}
	inline double cross(const Vector &v) const {
		return x*v.y - y*v.x;
	}

	inline double cos(const Vector &v) const {
		return dot(v) / size() / v.size();
	}
	inline double sin(const Vector &v) const {
		return cross(v) / size() / v.size();
	}

	inline Vector unit() const {
		return (*this) / size();
	}
	inline void normalize() {
		(*this) /= size();
	}

	double x, y;

};

inline Vector operator * (double a, const Vector& v) {
	return v*a;
}

typedef Vector Point;

#endif
