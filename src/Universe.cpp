#include "Universe.h"

using namespace std;
using namespace boost;

Universe::Universe():
		settings(), planets(), ships(), bullets() {}

void Universe::update(list<pair<uint16_t, uint16_t>> &hits) {
	vector<size_t> toErase;
	for (Bullet &b : bullets) {
		if (b.active()) {
			if (b.update(planets, ships, hits)) {
				toErase.push_back(b.id());
			}
		}
	}
	for (size_t i : toErase) {
		bullets.free(bullets[i]);
	}
}
