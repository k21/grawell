#include "Universe.h"

using namespace std;

Universe::Universe():
		settings(), planets(), ships(), bullets() {}

void Universe::update(list<unsigned short> &destroyed, double dt) {
	list<Bullet>::iterator it = bullets.begin();
	while (it != bullets.end()) {
		if (it->update(planets, ships, destroyed, dt)) {
			it = bullets.erase(it);
		} else ++it;
	}
}
