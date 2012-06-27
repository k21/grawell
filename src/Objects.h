#ifndef OBJECTS_H_
#define OBJECTS_H_

#include <list>
#include <vector>
#include <string>

#include <boost/cstdint.hpp>

#include <SFML/Graphics.hpp>

#include "Entity.h"

class Planet : public Entity {

public:
	Planet(const Point &pos_, Vector::T radius_, boost::int32_t mass_):
		Entity(pos_, radius_), mass(mass_) {}

	void draw(sf::RenderTarget &target) const;

	boost::int32_t mass;

};

class Bullet;

class Ship : public Entity {

public:
	Ship(boost::uint16_t id_): Entity(Point(0,0), 15*FIXED_ONE), id(id_),
			direction(0), strength(20000),
			active(false), alive(false), connected(false),
			ready(false),
			name(), score(0) {}

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

	bool active, alive, connected;
	bool ready;
	std::string name;
	boost::int32_t score;

};

class Bullet : public Entity {

public:
	Bullet(boost::uint16_t playerID_, const Point &pos,
			const Vector &speed_): Entity(pos, 2*FIXED_ONE),
			playerID(playerID_), speed(speed_) {}

	bool update(const std::vector<Planet> &planets, std::vector<Ship> &ships,
			std::list<std::pair<boost::uint16_t, boost::uint16_t>> &hits);
	void draw(sf::RenderTarget &target) const;

	boost::uint16_t playerID;
	Vector speed;

};

#endif
