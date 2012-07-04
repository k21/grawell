#ifndef PLANET_H_
#define PLANET_H_

#include <boost/cstdint.hpp>

#include <SFML/Graphics.hpp>

#include "Entity.h"
#include "EntityManager.h"

class Planet : public Entity {

public:
	void draw(sf::RenderTarget &target) const;

	boost::int32_t mass;

protected:
	Planet(boost::uint16_t id_): Entity(id_), mass(0) {}

	friend class EntityManager<Planet>;

};

#endif
