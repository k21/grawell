#ifndef UNIVERSE_H_
#define UNIVERSE_H_

#include <list>
#include <utility>
#include <vector>

#include <cstdint>

#include "Bullet.h"
#include "Planet.h"
#include "Settings.h"
#include "Ship.h"
#include "Trail.h"

class Universe {

public:
	Universe();

	void update(std::list<std::pair<uint16_t, uint16_t>> &hits,
			bool updateTrails, std::list<Trail> *trails);

	Settings settings;
	EntityManager<Planet> planets;
	EntityManager<Ship>   ships;
	EntityManager<Bullet> bullets;

};

#endif
