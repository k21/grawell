#ifndef OBJECTS_H_
#define OBJECTS_H_

#include <SFML/Graphics.hpp>

#include "Sphere.h"

class Object : public Sphere {
public:
	Object(const Point &pos, double radius): Sphere(pos, radius) {}
	virtual void draw(sf::RenderTarget &target) const = 0;
	virtual ~Object() {};
};

class Planet : public Object {

public:
	Planet(const Point &pos, double radius, double mass):
		Object(pos, radius), mass_(mass) {}
	double mass() const { return mass_; }
	void mass(double m) { mass_ = m; }

	void draw(sf::RenderTarget &target) const;

private:
	double mass_;

};

class Ship : public Object {

public:
	

private:
	

};

class Bullet : public Object {

public:
	

private:
	

};

#endif
