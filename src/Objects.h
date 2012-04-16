#ifndef OBJECTS_H_
#define OBJECTS_H_

#include <list>
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
	Ship(unsigned short id_, const Point &pos): Sphere(pos, 15), id(id_),
			direction(0), strength(20000),
			active(false), name(), score(0), ready(true) {}

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

	unsigned short id;

	unsigned short direction;
	unsigned long strength;

	bool active;
	std::string name;
	long score;
	bool ready;

};

class Bullet : public Sphere {

public:
	Bullet(unsigned short playerID_, const Point &pos,
			const Vector &speed_):
		Sphere(pos, 2), playerID(playerID_), speed(speed_) {}

	bool update(const std::vector<Planet> &planets, std::vector<Ship> &ships,
			std::list<unsigned short> &destroyed, double dt);
	void draw(sf::RenderTarget &target) const;

	unsigned short playerID;
	Vector speed;

};

#endif
