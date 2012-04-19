#ifndef OBJECTS_H_
#define OBJECTS_H_

#include <list>
#include <vector>
#include <string>

#include <boost/cstdint.hpp>

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
	Ship(boost::uint16_t id_): Sphere(Point(0,0), 15), id(id_),
			direction(0), strength(20000),
			active(false), deactivate(false),
			name(), score(0), ready(true) {}

	Bullet shoot() const;
	void draw(sf::RenderTarget &target) const;
	void rotate(boost::int32_t d) {
		if (d == 0) return;
		if (d > 0) {
			direction = (boost::uint16_t)(direction + d);
			if (direction >= 36000) {
				direction = (boost::uint16_t)(direction - 36000);
			}
		}
		if (d < 0) {
			d = -d;
			if (direction < d) {
				direction = (boost::uint16_t)(36000 - d);
			} else direction = (boost::uint16_t)(direction - d);
		}
	}

	boost::uint16_t id;

	boost::uint16_t direction;
	boost::uint32_t strength;

	bool active, deactivate;
	std::string name;
	boost::int32_t score;
	bool ready;

};

class Bullet : public Sphere {

public:
	Bullet(boost::uint16_t playerID_, const Point &pos,
			const Vector &speed_):
		Sphere(pos, 2), playerID(playerID_), speed(speed_) {}

	bool update(const std::vector<Planet> &planets, std::vector<Ship> &ships,
			std::list<std::pair<boost::uint16_t, boost::uint16_t>> &hits,
			double dt);
	void draw(sf::RenderTarget &target) const;

	boost::uint16_t playerID;
	Vector speed;

};

#endif
