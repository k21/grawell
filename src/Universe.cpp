#include "Universe.h"

using namespace std;
using namespace boost;

Universe::Universe():
		settings(), planets(), ships(), bullets() {}

void Universe::update(list<uint16_t> &destroyed, double dt) {
	auto it = bullets.begin();
	while (it != bullets.end()) {
		if (it->update(planets, ships, destroyed, dt)) {
			it = bullets.erase(it);
		} else ++it;
	}
}
