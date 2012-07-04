#ifndef BULLET_H_
#define BULLET_H_

#include <list>

#include <boost/cstdint.hpp>

#include <SFML/Graphics.hpp>

#include "Entity.h"
#include "EntityManager.h"
#include "Planet.h"
#include "Ship.h"
#include "Trail.h"

class Bullet : public Entity {

public:
	bool update(EntityManager<Planet> &planets, EntityManager<Ship> &ships,
			std::list<std::pair<boost::uint16_t, boost::uint16_t>> &hits,
			bool updateTrails);
	void draw(sf::RenderTarget &target) const;

	boost::uint16_t owner() const { return myOwner; }
	void owner(boost::uint16_t pid) {
		myOwner = pid;
		sf::Color trailColor = Ship::getColorByID(pid);
		trailColor.a = 96;
		trail.setColor(trailColor);
	}

	Vector speed;

	Trail trail;

protected:
	Bullet(boost::uint16_t id_): Entity(id_), speed(),
			trail(1000, sf::Color(0,0,0,0)), myOwner(65535) {
		radius = 2*FIXED_ONE;
	}

	boost::uint16_t myOwner;

	friend class EntityManager<Bullet>;

};

#endif
