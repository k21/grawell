#ifndef OBJECTS_H_
#define OBJECTS_H_

#include <vector>

#include <SFML/Graphics.hpp>

#include "Sphere.h"

class Planet : public Sphere {

public:
	Planet(const Point &pos, double radius, double mass_):
		Sphere(pos, radius), mass(mass_) {}

	void draw(sf::RenderTarget &target) const;

	double mass;

};

class Ship : public Sphere {

public:
	Ship(const Point &pos): Sphere(pos, 15), dir(0) {}

	void draw(sf::RenderTarget &target) const;

	int dir;

};

class Bullet : public Sphere {

public:
	Bullet(const Point &pos, const Vector &speed_):
		Sphere(pos, 2), speed(speed_) {}

	bool update(const std::vector<Planet> &planets, double dt);
	void draw(sf::RenderTarget &target) const;

	Vector speed;

};

#endif
