#ifndef UNIVERSE_H_
#define UNIVERSE_H_

#include <cstdint>
#include <list>
#include <utility>
#include <vector>

#include "Bullet.h"
#include "Planet.h"
#include "Settings.h"
#include "Ship.h"
#include "Trail.h"

#define MAX_STRENGTH 25000

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
