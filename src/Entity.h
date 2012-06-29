#ifndef ENTITY_H_
#define ENTITY_H_

#include <boost/cstdint.hpp>

#include "Vector.h"

class Entity {

public:
	virtual ~Entity() {}

	boost::uint16_t id() const { return myID; }
	bool active() const { return myActive; }

	bool intersects(const Point &p) const {
		return (center - p).size() <= radius;
	}
	bool intersects(const Entity &e) const {
		return (center - e.center).size() <= radius + e.radius;
	}

	Point center;
	Vector::T radius;

protected:
	Entity(boost::uint16_t id_): center(0, 0), radius(0),
			myID(id_), myActive(false) {}
	boost::uint16_t myID;
	bool myActive;

};

#endif
