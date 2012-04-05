#ifndef SPHERE_H_
#define SPHERE_H_

#include "Vector.h"

class Sphere {

public:
	Sphere(const Point &center, double radius): c(center), r(radius) {}
	inline Point &center() { return c; }
	inline const Point &center() const { return c; }
	inline void center(const Point &center) { c = center; }
	inline double radius() const { return r; }
	inline void radius(double radius) { r = radius; }
	inline double x() const { return c.x(); }
	inline void x(double x) { c.x(x); }
	inline double y() const { return c.y(); }
	inline void y(double y) { c.y(y); }

	bool intersects(Point &p) const {
		return (c - p).size() <= r;
	}
	bool intersects(Sphere &s) const {
		return (c - s.c).size() <= r + s.r;
	}


private:
	Point c;
	double r;

};

#endif
