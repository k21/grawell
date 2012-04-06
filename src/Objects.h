#ifndef OBJECTS_H_
#define OBJECTS_H_

#include <vector>

#include <SFML/Graphics.hpp>

#include "Sphere.h"

class Planet : public Sphere {

public:
	Planet(const Point &pos, double radius, double mass):
		Sphere(pos, radius), mass_(mass) {}
	double mass() const { return mass_; }
	void mass(double m) { mass_ = m; }

	void draw(sf::RenderTarget &target) const;

private:
	double mass_;

};

class Ship : public Sphere {

public:
	Ship(const Point &pos): Sphere(pos, 15), dir(0) {}

	void draw(sf::RenderTarget &target) const;

private:
	double dir;

};

class Bullet : public Sphere {

public:
	Bullet(const Point &pos, const Vector &speed):
		Sphere(pos, 2), speed_(speed) {}

	bool update(const std::vector<Planet> &planets);
	void draw(sf::RenderTarget &target) const;

private:
	Vector speed_;

};

#endif
