#include "Universe.h"

using namespace std;

Universe::Universe():
		settings(), planets(), ships(), bullets() {}

void Universe::update(double dt) {
	list<Bullet>::iterator it = bullets.begin();
	while (it != bullets.end()) {
		if (it->update(planets, dt)) {
			it = bullets.erase(it);
		} else ++it;
	}
}
