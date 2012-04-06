#include "Universe.h"

using namespace std;

Universe::Universe() {
	ships.push_back(Ship(Point(100,100)));
	planets.push_back(Planet(Point(400,300),20,30000000));
}

void Universe::update(double dt) {
	list<Bullet>::iterator it = bullets.begin();
	while (it != bullets.end()) {
		if (it->update(planets, dt)) {
			list<Bullet>::iterator it2 = it; ++it2;
			bullets.erase(it);
			it = it2;
		} else ++it;
	}
}
