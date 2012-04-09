#ifndef SPHERE_H_
#define SPHERE_H_

#include "Vector.h"

class Sphere {

public:
	Sphere(const Point &center_, double radius_):
		center(center_), radius(radius_) {}
	virtual ~Sphere() {}

	bool intersects(const Point &p) const {
		return (center - p).size() <= radius;
	}
	bool intersects(const Sphere &s) const {
		return (center - s.center).size() <= radius + s.radius;
	}

	Point center;
	double radius;

};

#endif
