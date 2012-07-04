#ifndef SHIP_H_
#define SHIP_H_

#include <string>

#include <boost/cstdint.hpp>

#include <SFML/Graphics.hpp>

#include "Entity.h"
#include "EntityManager.h"

class Bullet;

class Ship : public Entity {

public:
	void shoot(EntityManager<Bullet> &bullets) const;
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
				direction = (boost::uint16_t)(36000 - d + direction);
			} else direction = (boost::uint16_t)(direction - d);
		}
	}

	static sf::Color getColorByID(boost::uint16_t id);

	boost::uint16_t direction;
	boost::uint32_t strength;

	bool inGame, alive, connected;
	bool ready;
	std::string name;
	boost::int32_t score;

protected:
	Ship(boost::uint16_t id_): Entity(id_),
			direction(0), strength(20000),
			inGame(false), alive(false), connected(false),
			ready(false),
			name(), score(0) {
		radius = 15*FIXED_ONE;
	}

	friend class EntityManager<Ship>;

};

#endif
