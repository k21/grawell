#ifndef UNIVERSE_H_
#define UNIVERSE_H_

#include <list>
#include <utility>
#include <vector>

#include <boost/cstdint.hpp>

#include "Bullet.h"
#include "Planet.h"
#include "Settings.h"
#include "Ship.h"
#include "Trail.h"

class Universe {

public:
	Universe();

	void update(std::list<std::pair<boost::uint16_t, boost::uint16_t>> &hits,
			bool updateTrails, std::vector<Trail> *trails);

	Settings settings;
	EntityManager<Planet> planets;
	EntityManager<Ship>   ships;
	EntityManager<Bullet> bullets;

};

#endif
