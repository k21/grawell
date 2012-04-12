#ifndef OBJECTS_H_
#define OBJECTS_H_

#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

#include "Sphere.h"

class Planet : public Sphere {

public:
	Planet(const Point &pos_, double radius_, double mass_):
		Sphere(pos_, radius_), mass(mass_) {}

	void draw(sf::RenderTarget &target) const;

	double mass;

};

class Bullet;

class Ship : public Sphere {

public:
	Ship(const Point &pos): Sphere(pos, 15),
			direction(0), strength(20000),
			active(false), name(), score(0) {}

	Bullet shoot() const;
	void draw(sf::RenderTarget &target) const;
	void rotate(int d) {
		if (d == 0) return;
		if (d > 0) {
			direction = (unsigned short)(direction + d);
			if (direction >= 36000) {
				direction = (unsigned short)(direction - 36000);
			}
		}
		if (d < 0) {
			d = -d;
			if (direction < d) {
				direction = (unsigned short)(36000 - d);
			} else direction = (unsigned short)(direction - d);
		}
	}

	unsigned short direction;
	unsigned long strength;

	bool active;
	std::string name;
	long score;

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
