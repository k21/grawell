#ifndef VECTOR_H_
#define VECTOR_H_

#include <cmath>

class Vector {

public:
	Vector(): x_(0), y_(0) {}
	Vector(double x, double y): x_(x), y_(y) {}
	inline double x() const { return x_; }
	inline double y() const { return y_; }
	inline void x(double nx) { x_ = nx; }
	inline void y(double ny) { y_ = ny; }

	inline Vector operator + (const Vector &v) const {
		return Vector(x_+v.x_, y_+v.y_);
	}
	inline Vector operator - (const Vector &v) const {
		return Vector(x_-v.x_, y_-v.y_);
	}
	inline Vector operator - () const {
		return Vector(-x_, -y_);
	}
	inline Vector operator * (double a) const {
		return Vector(x_*a, y_*a);
	}
	inline Vector operator / (double a) const {
		return Vector(x_/a, y_/a);
	}

	inline const Vector &operator += (const Vector &v) {
		x_ += v.x_; y_ += v.y_; return *this;
	}
	inline const Vector &operator -= (const Vector &v) {
		x_ -= v.x_; y_ -= v.y_; return *this;
	}
	inline const Vector &operator *= (double a) {
		x_ *= a; y_ *= a; return *this;
	}
	inline const Vector &operator /= (double a) {
		x_ /= a; y_ /= a; return *this;
	}

	inline double sizeSqr() const {
		return x_*x_ + y_*y_;
	}
	inline double size() const {
		return sqrt(x_*x_ + y_*y_);
	}
	inline double dir() const {
		return atan2(y_, x_);
	}

	inline double dot(const Vector &v) const {
		return x_*v.x_ + y_*v.y_;
	}
	inline double cross(const Vector &v) const {
		return x_*v.y_ - y_*v.x_;
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

private:
	double x_, y_;

};

inline Vector operator * (double a, const Vector& v) {
	return v*a;
}

typedef Vector Point;

#endif
