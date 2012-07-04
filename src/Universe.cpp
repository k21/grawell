#include "Universe.h"

using namespace std;
using namespace boost;

Universe::Universe():
		settings(), planets(), ships(), bullets() {}

void Universe::update(list<pair<uint16_t, uint16_t>> &hits, bool updateTrails,
		vector<Trail> *trails) {
	vector<size_t> toErase;
	for (Bullet &b : bullets) {
		if (b.active()) {
			if (b.update(planets, ships, hits, updateTrails)) {
				toErase.push_back(b.id());
			}
		}
	}
	for (size_t i : toErase) {
		if (updateTrails) {
			Trail &t = bullets[i].trail;
			trails->push_back(Trail(t.getMaxSize(), t.getColor()));
			t.swapPoints(trails->back());
		}
		bullets.free(bullets[i]);
	}
}
