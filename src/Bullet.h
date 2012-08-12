#ifndef BULLET_H_
#define BULLET_H_

#include <cstdint>
#include <list>

#include <SFML/Graphics.hpp>

#include "Entity.h"
#include "EntityManager.h"
#include "Planet.h"
#include "Ship.h"
#include "Trail.h"

class Bullet : public Entity {

public:
	bool update(EntityManager<Planet> &planets, EntityManager<Ship> &ships,
			std::list<uint16_t> &hits,
			bool updateTrails);
	void draw(sf::RenderTarget &target) const;

	uint16_t owner() const { return myOwner; }
	void owner(uint16_t pid) {
		myOwner = pid;
		sf::Color trailColor = Ship::getColorByID(pid);
		trailColor.r = (sf::Uint8)((float)trailColor.r * 0.4f);
		trailColor.g = (sf::Uint8)((float)trailColor.g * 0.4f);
		trailColor.b = (sf::Uint8)((float)trailColor.b * 0.4f);
		trail.setColor(trailColor);
	}

	uint32_t originalPower;

	Vector speed;

	Trail trail;

protected:
	Bullet(uint16_t id_): Entity(id_), originalPower(0), speed(),
			trail(1000, sf::Color(0,0,0,0)), myOwner(65535) {
		radius = 8*FIXED_ONE;
	}

	uint16_t myOwner;

	friend class EntityManager<Bullet>;

};

#endif
