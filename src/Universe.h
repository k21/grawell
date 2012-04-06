#ifndef UNIVERSE_H_
#define UNIVERSE_H_

#include <list>
#include <vector>

#include "Objects.h"
#include "Settings.h"

class Universe {

public:
	Universe();

	void update(double dt);

	Settings settings;
	std::vector<Planet> planets;
	std::vector<Ship>   ships;
	std::list<Bullet>   bullets;

};

#endif
