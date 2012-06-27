#ifndef ENTITY_H_
#define ENTITY_H_

#include "Vector.h"

class Entity {

public:
	Entity(const Point &center_, Vector::T radius_):
		center(center_), radius(radius_) {}
	virtual ~Entity() {}

	bool intersects(const Point &p) const {
		return (center - p).size() <= radius;
	}
	bool intersects(const Entity &e) const {
		return (center - e.center).size() <= radius + e.radius;
	}

	Point center;
	Vector::T radius;

};

#endif
