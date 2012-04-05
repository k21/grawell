#ifndef VECTOR_H_
#define VECTOR_H_

#include <cmath>

class Vector {

public:
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

	inline const Vector &operator += (const Vector &v) {
		x_ += v.x_; y_ += v.y_; return *this;
	}
	inline const Vector &operator -= (const Vector &v) {
		x_ -= v.x_; y_ -= v.y_; return *this;
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

private:
	double x_, y_;

};

typedef Vector Point;

#endif
