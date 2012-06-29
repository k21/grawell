#ifndef ENTITIES_H_
#define ENTITIES_H_

#include <list>
#include <vector>
#include <string>

#include <boost/cstdint.hpp>

#include <SFML/Graphics.hpp>

#include "EntityManager.h"

#include "Entity.h"

class Planet : public Entity {

public:
	void draw(sf::RenderTarget &target) const;

	boost::int32_t mass;

protected:
	Planet(boost::uint16_t id_): Entity(id_), mass(0) {}

	friend class EntityManager<Planet>;

};

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
				direction = (boost::uint16_t)(36000 - d);
			} else direction = (boost::uint16_t)(direction - d);
		}
	}

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

class Bullet : public Entity {

public:
	bool update(EntityManager<Planet> &planets, EntityManager<Ship> &ships,
			std::list<std::pair<boost::uint16_t, boost::uint16_t>> &hits,
			bool updateTrails);
	void draw(sf::RenderTarget &target) const;

	boost::uint16_t playerID;
	Vector speed;

	std::vector<Point> trail;

protected:
	Bullet(boost::uint16_t id_): Entity(id_), playerID(65535), speed(), trail() {
		radius = 2*FIXED_ONE;
	}

	friend class EntityManager<Bullet>;

};

#endif
